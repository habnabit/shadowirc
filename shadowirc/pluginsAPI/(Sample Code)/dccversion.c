/*	ShadowIRC Sample Plugin
		© 1997-1998 John Bafford
		Permission is granted to use this code in any plugin.
*/

#include "ShadowIRC.h"

void setupMessages(char captureMessages[numMessages]);
void checkDCCVersion(ShadowIRCDataRecord* sidr, pDCCChatCookieDataPtr p);

 /*	We don't set sidr->completelyProcessed or p->processed because every plugin might
 		be interested in the version cookie. Also, ShadowIRC has already processed it.
 		*DO NOT EVER* reply to the 'SIDR' cookie.
*/

void checkDCCVersion(ShadowIRCDataRecord* sidr, pDCCChatCookieDataPtr p)
{
	#pragma unused(sidr)
	 dccCHATDataPtr dd = (dccCHATDataPtr)((dccPtr)p->conn->dcc)->dccData;
	
	if(p->cookie == 'SIRC')
	{
		short major, minor, rev, build;
		char stage;
		unsigned char s[10];
		LongString ls;
		long vers = *(long*)&p->data->data[1];
		
		major = vers >> 24;
		minor = (vers >> 16) % 256;
		rev = (vers >> 8) % 256;
		if(vers & 0x80)
		{
			stage = 'r';
			build = (vers % 256) - 0x80;
		}
		else
		{
			stage = 'd';
			build = vers % 256;
		}
		
		s[1] = major + '0';
		s[2] = '.';
		s[3] = minor + '0';
		s[4] = '.';
		s[5] = rev + '0';
		if(build)
		{
			NumToString(build, &s[6]);
			s[0]=6+s[6];
			s[6] = stage;
		}
		else
			s[0]=5;
		
		LSConcatStrAndStr("\pRemote user is running ShadowIRC ", s, &ls);
		SMPrefix(&ls, dsNowhere);
		MWMessage((MWPtr)dd->dccWindow, &ls);
	}
}


void setupMessages(char captureMessages[numMessages])
{
	captureMessages[pDCCChatCookieMessage]=1;
}

pascal void main(ShadowIRCDataRecord* sidr)
{
	#if !__POWERPC__
	EnterCodeResource();
	#endif

	switch(sidr->message)
	{
		case pVersionCheckMessage:
			if(((pVersionCheckDataPtr)sidr->messageData)->version<0x00080030) //we must have ShadowIRC 0.8.0d40 or later.
				((pVersionCheckDataPtr)sidr->messageData)->version = pVersionShadowIRCTooOld;
			else
				((pVersionCheckDataPtr)sidr->messageData)->version = pVersionCheckMessageReply;
			break;
			
		case pInitMessage:
			setupMessages(sidr->yourInfo->captureMessages);
			break;
		
		case pDCCChatCookieMessage:
			checkDCCVersion(sidr, (pDCCChatCookieDataPtr)sidr->messageData);
			break;
		
	}
	
	#if !__POWERPC__
	ExitCodeResource();
	#endif
}