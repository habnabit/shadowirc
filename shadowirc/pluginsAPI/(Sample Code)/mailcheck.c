/*	mailcheck.c, ©ÊJohn Bafford 1998
		ShadowIRC Sample Plugin
		Version 1.2
*/

/*	This plugin demonstrates the use of inputwindow widgets, DNS lookups, preferences files and
		preferences window use, TCP/IP connections, and IPC (inter-plugin communication).
*/

#include "ShadowIRC.h"

typedef struct mailPrefs {
	Str255 username, password, server;
	short timer;
} mailPrefs, *mailPrefsPtr;

pascal void popQuit(void);
pascal void processPOP(LongString *data);
pascal void startmail(void);
pascal void startmail2(void);
pascal void CheckDNSResponse(ShadowIRCDataRecord* sidr, pDNSLookupPtr p);
pascal void handleConnection(ShadowIRCDataRecord* sidr, pTextConnectionData *p);
pascal char parseCommand(ShadowIRCDataRecord* sidr, pCommandDataRec *p);

pascal void LoadPrefs(void);
pascal void prefsWindowSet(ShadowIRCDataRecord* sidr, pPWSetWindowDataPtr p);
pascal void prefsWindowGet(ShadowIRCDataRecord* sidr, pPWGetWindowDataPtr p);

pascal void DrawIW(ShadowIRCDataRecord* sidr, pIWUpdateData *p);
pascal void ClickIW(ShadowIRCDataRecord* sidr, pIWUpdateData *p);
pascal void SetupStatusLine(ShadowIRCDataRecord* sidr);
pascal void IPC(ShadowIRCDataRecord* sidr, pIPCMessageData *p);

#define kIPLookup 'ipma'
#define kEmailConn 'mail'
#define prefsName "\pMail Check Preferences"

enum {
	kMailStatus = 'mail'
};

enum {
	kMailcheckIPC = 'mail',
	kMailcheckIPCVersion = 0,
	
	kIPCGetMailNum = 0,
	kIPCGetMailSize,
	kIPCGetMailStat,
	kIPCGetMailConnectTime,
	kIPCCheckMail
};

enum connectStageList
{
	kIdle = -1,
	kLookingUp,
	kReadyToStart,
	kPasswordStage,
	kStatStage,
	
	kQuitStage
};

enum mailStatusList
{
	kUnknown,
	kKnown
};

iwWidgetPtr mailIWObj;
short mailStatus = kUnknown, mailNum = 0;
long mailSize=0;
connectionPtr conn = 0;
long ip;
mailPrefsPtr prefs;
short prefFile;
short prefsPanel;
unsigned long lastConnect;
ShadowIRCDataRecord* sidr; //For 68k plugins, so we don't have to pass sidr all over the place in the PPC version for no reason at all.

pascal void popQuit(void) //disconnect from the POP server by sending the quit command
{
	LongString ls;
	
	LSStrLS("\pQUIT", &ls);
	ConnPutLS(&conn, &ls);
}

pascal void processPOP(LongString *data) //process text from the server
{
	LongString ls;
	Str255 s, s2;
	long n;

	//Remove the cr/lf at the end of the text.
	while(data->len)
	{
		if(	data->data[data->len] == 10 || data->data[data->len] == 13)
			data->len--;
		else
			break;
	}
	switch(conn->connectStage)
	{
		case kReadyToStart: //send USER + name
			LSConcatStrAndStr("\pUSER ", prefs->username, &ls);
			conn->connectStage++;
			ConnPutLS(&	conn, &ls);
			break;
		
		case kPasswordStage: //send PASS + pwd
			if(data->data[1]=='-') //it didn't like the username. That's a bad thing.
			{
				LSStrLS("\pmailcheck: Unknown username.", &ls);
				SMPrefix(&ls, dsConsole);
				conn->connectStage=kIdle;
				popQuit();
			}
			else
			{
				LSConcatStrAndStr("\pPASS ", prefs->password, &ls);
				conn->connectStage++;
				ConnPutLS(&conn, &ls);
			}
			break;
		
		case kStatStage:
			if(data->data[1]=='-') //it didn't like the password.
			{
				LSStrLS("\pmailcheck: Invalid password.", &ls);
				SMPrefix(&ls, dsConsole);
				conn->connectStage=kIdle;
				popQuit();
			}
			else
			{
				LSStrLS("\pSTAT", &ls);
				conn->connectStage++;
				ConnPutLS(&conn, &ls);
			}
			break;
		
		case kQuitStage: //yank info from stat and quit
			LSNextArg(data, 0); //+OK 
			LSNextArg(data, s);
			LSNextArg(data, s2);
			StringToNum(s, &n);
			mailNum = n;
			mailStatus=kKnown;
			StringToNum(s2, &mailSize);
			if(n)
			{
//				LSStrCat(5, &ls,"\pYou have ", s, "\p messages (", s2, "\p bytes)");
//				MWMessage(win,&ls);
			}
			else
			{
//				LSStrLS("\pYou have no messages.", &ls);
//				MWMessage(win, &ls);
			}
			conn->connectStage=kIdle;
			popQuit();
			break;
	}
}

pascal void startmail(void) //begin mail check
{
	if(prefs->username[0] && prefs->password[0] && prefs->server[0]) //don't check if they haven't set everything.
	{
		if(!conn)
		{
			DNSLookup(prefs->server, kIPLookup);
			conn=pluginNewConnection(true);
			conn->connectStage = kLookingUp;
		}
	}
}

pascal void startmail2(void) //open the connection
{
	LongString ls;
	
	if(!conn->socket)
	{
		conn->refCon=kEmailConn;
		conn->ip=ip;
		conn->port=110;
		conn->outgoing=true;
		conn->connectStage=kReadyToStart;
		conn->lineTerm=eolCRLF;
		if(ConnNewActive(conn))
			LSStrLS("\pStarted connection...", &ls);
		else
			LSStrLS("\pCouldn't start connection...", &ls);
	}
	else
		LSStrLS("\pConnection already open!", &ls);
	
//	MWMessage(win, &ls);
}

pascal void CheckDNSResponse(ShadowIRCDataRecord* sidr, pDNSLookupPtr p)
{
	#pragma unused(sidr)
	LongString ls;
	
	GetDateTime(&lastConnect);
	if(p->successful)
	{
		if(p->refCon==kIPLookup)
		{
			ip=p->ip;
			startmail2();
		}
	}
	else
	{
		LSConcatStrAndStr("\pmailcheck: Couldn't lookup ", p->search, &ls);
		SMPrefix(&ls, dsConsole);
		conn->connectStage = kIdle;
	}
}

pascal void handleConnection(ShadowIRCDataRecord* sidr, pTextConnectionData *p)
{
	#pragma unused(sidr)
//	Str255 s;
//	LongString ls;
	
	switch(p->event)
	{
		case 0:
			break;
			
		case C_Established:
//			LSStrLS("\pConnection established", &ls);
//			MWMessage(win, &ls);
			break;
			
		case C_CharsAvailable:
			processPOP(p->data);
			break;
		
		case C_Closing:
		case C_Closed:
//			LSStrLS("\pConnection closing", &ls);
//			MWMessage(win, &ls);
			ConnAbort(conn);
			deleteConnection(&conn);
			break;
	}
}

pascal char parseCommand(ShadowIRCDataRecord* sidr, pCommandDataRec *p)
{
	#pragma unused(sidr)
	
	if(pstrcmp(p->com, "\pMAILCHECK"))
	{
		startmail();
		p->outgoing->len=0;
	}
	return 0;
}

pascal void prefsWindowSet(ShadowIRCDataRecord* sidr, pPWSetWindowDataPtr p)
{
	Str255 s;
	
	#pragma unused(sidr)
	if(p->id==prefsPanel)
	{
		SetText(p->PrefsDlg, 7, prefs->username);
		SetText(p->PrefsDlg, 8, prefs->password);
		SetText(p->PrefsDlg, 9, prefs->server);
		NumToString(prefs->timer, s);
		SetText(p->PrefsDlg, 11, s);
	}
}

pascal void prefsWindowGet(ShadowIRCDataRecord* sidr, pPWGetWindowDataPtr p)
{
	#pragma unused(sidr)
	long l;
	short err;
	Str255 s;
	
	if(p->id==prefsPanel)
	{
		GetText(p->PrefsDlg, 7, prefs->username);
		GetText(p->PrefsDlg, 8, prefs->password);
		GetText(p->PrefsDlg, 9, prefs->server);
		GetText(p->PrefsDlg, 11, s);
		StringToNum(s, &l);
		prefs->timer=l;
		err=PFSetPos(prefFile, 0);
		l=sizeof(mailPrefs);
		err=PFWrite(prefFile, (Ptr)prefs, &l);
	}
}

pascal void LoadPrefs(void)
{
	short err;
	LongString ls;
	char alert = 0;
	
	prefs=(mailPrefsPtr)NewPtr(sizeof(mailPrefs));
	
	if(PFExists(prefsName))
	{
		long l = sizeof(mailPrefs);
		err=PFOpen(prefsName, false, &prefFile, 0);
		err=PFSetPos(prefFile, 0);
		err=PFRead(prefFile, (Ptr)prefs, &l);
	}
	else
	{
		prefs->username[0]=0;
		prefs->password[0]=0;
		prefs->server[0]=0;
		prefs->timer=5;
		err=PFCreate(prefsName, 'pref', 'SIRC', false);
		err=PFOpen(prefsName, false, &prefFile, 0);
		err=PFResize(prefFile, sizeof(mailPrefs));
		/*	We could open a preferences panel now, but that's probabally not the nicest thing to do
				as it might seem intrusive, although it does have it's neatness factor.
		OpenPreferencesWindow(prefsPanel);
		*/
	
		alert = 1;
	}
	
	if(alert || prefs->username[0]==0 || prefs->password[0]==0 || prefs->server[0]==0)
	{
		LSStrLS("\pPlease set your mail preferences in the 'mailcheck' preferences panel.", &ls);
		SMPrefixIrcleColor(&ls, dsFrontWin, 2); //red
	}
}

pascal void DrawIW(ShadowIRCDataRecord* sidr, pIWUpdateData *p)
{
	#pragma unused(sidr)
	Str255 s;
	//the port is already the inputline, and the current pen position is at a position where we can
	//just DrawString() our text directly into the status line.

	if(p->widget->type == kMailStatus) //This isn't really necessary, but if we were to create more than one inputline object, we would need to check
	{
		DrawString("\pMail: ");
		if(mailStatus == kUnknown)
			DrawString("\p???");
		else
		{
			NumToString(mailNum, s);
			DrawString(s);
		}
	}
}

pascal void ClickIW(ShadowIRCDataRecord* sidr, pIWUpdateData *p)
{
	#pragma unused(sidr)
	MenuHandle m;
	long res;
	Point pt;
	short mid, item;
	Str255 s;
	LongString ls;
	
	if(p->widget->type == kMailStatus)
	{
		//Just for fun, let's toss up a menu that displays the current connection status,
		//number of messages, size of the messages, and an option to connect
		
		m = NewMenu(150, "\p"); //create a new menu
		if(!conn || (conn->connectStage == kIdle))
			AppendMenu(m,"\pCheck Mail;-"); //Add a connect item and a seperator
		else //we're attempting to connect
		{
			AppendMenu(m, "\pChecking Mail;-");
			DisableItem(m, 1);
		}
		
		//This is not the most optimal way memory wise to concatenate Str255s, but for all we need.
		//this is the easiest way. (I was too lazy to write a pstrcat() function.)
		if(mailStatus == kUnknown)
			*(long*)s=0x033F3F3F; //"\p???"
		else
			NumToString(mailNum, s);
		LSConcatStrAndStr("\pMail: ", s, &ls);
		if(ls.len>255)
			ls.data[0]=255;
		else
			ls.data[0]=ls.len;
		AppendMenu(m, ls.data);
		
		NumToString(mailSize, s);
		LSConcatStrAndStrAndStr("\pSize: ", s, "\p bytes", &ls);
		if(ls.len>255)
			ls.data[0]=255;
		else
			ls.data[0]=ls.len;
		AppendMenu(m, ls.data);
		
		pt.h = p->widget->drawArea.left;
		pt.v = p->widget->drawArea.top;
		res = IWPopUpMenu(pt, m, 0);
		
		mid=*(short*)res; //the high two bytes are the menu id
		item = res & 0x0000FFFF; //the low two byes are the item that was selected
		
		if(item==1) //then they selected connect
		{
			if(!(prefs->username[0] && prefs->password[0] && prefs->server[0]))
				OpenPreferencesWindow(prefsPanel);
			startmail();
		}
		DeleteMenu(150);
	}
}

pascal void SetupStatusLine(ShadowIRCDataRecord* sidr)
{
	short wid;
	GrafPtr p0;
	
	GetPort(&p0);
	SetPort(sidr->inputLine->window);	//Set the current graphics port to the inputline

	TextFace(0);	//We want plain text
	wid=StringWidth("\pMail: 0000"); //Request a width equal to the string 'Mail: ' and four numbers
	mailIWObj = IWNewWidget(kMailStatus, iwAlignRight, wid);
	mailIWObj->data = StringWidth("\pMail: "); //Save the width of the header, just in case we need it
	
	SetPort(p0); //Set the port back to whatever it was before.
}

//Mailcheck listens for IPC messages of type 'mail'.
//It responds
pascal void IPC(ShadowIRCDataRecord* sidr, pIPCMessageData *p)
{
#pragma unused(sidr)
	if(p->type == kMailcheckIPC)
	{
		p->ack = true; //Assume we process it...
		switch(p->message)
		{
			case kIPCGetMailNum:
				p->result = mailNum;
				break;
			case kIPCGetMailSize:
				p->result = mailSize;
				break;
			case kIPCGetMailStat:
				p->result = mailStatus;
				break;
			case kIPCGetMailConnectTime:
				p->result = lastConnect;
				break;
			case kIPCCheckMail:
				startmail();
				break;

			default:
				p->ack = false; //we didn't process this message since we don't know what it is.
				break;
		}
	}
}

pascal void main(ShadowIRCDataRecord* sidrIN)
{
	unsigned long l;
	#if !__POWERPC__
	EnterCodeResource();
	#endif
	
	switch(sidrIN->message)
	{
		case pVersionCheckMessage:
			sidr=sidrIN; //This is valid here because sidr never changes its location in memory, so we only need to set our variable once.

			l=((pVersionCheckDataPtr)sidrIN->messageData)->version;
			if(l<0x0100007a) //we must have ShadowIRC 1.0fc6 or later.
				((pVersionCheckDataPtr)sidrIN->messageData)->version = pVersionShadowIRCTooOld;
			else
			{
				((pVersionCheckDataPtr)sidrIN->messageData)->version = pVersionCheckMessageReply;
			}
			break;
			
		case pInitMessage:
			sidrIN->yourInfo->captureMessages[pUserCommandMessage]=1;
			sidrIN->yourInfo->captureMessages[pIdleMessage]=1;
			sidrIN->yourInfo->idleThreshold= 30*60; //30 seconds
			
			//Add our preferences pane and load preferences.
			prefsPanel = PMLAdd("\pMail Check");
			LoadPrefs();
			
			//Create our stausline widget
			SetupStatusLine(sidrIN);
			
			//Register IPC services
			IPCRegister(kMailcheckIPC, kMailcheckIPCVersion, 0);
			//Throw away return value...we don't care.
			break;
		
		case pIWUpdateMessage:
			DrawIW(sidr, (pIWUpdateDataPtr)sidrIN->messageData);
			break;

		case pIWClickMessage:
			ClickIW(sidr, (pIWUpdateDataPtr)sidrIN->messageData);
			break;

		case pPWActivateMessage:
			if(((pPWActivateDataPtr)sidrIN->messageData)->id==prefsPanel)
			{
				((pPWActivateDataPtr)sidrIN->messageData)->ditlNum = 1533;
			}
			break;
		
		case pPWSetWindowMessage:
			prefsWindowSet(sidrIN, (pPWSetWindowDataPtr)sidrIN->messageData);
			break;
			
		case pPWGetWindowMessage:
			prefsWindowGet(sidrIN, (pPWGetWindowDataPtr)sidrIN->messageData);
			break;
			
		case pIdleMessage:
			if(!conn || !conn->socket)
			{
				GetDateTime(&l);
				if((l-lastConnect) > (prefs->timer * 60))
					startmail();
			}
			break;
		
		case pUserCommandMessage:
			sidrIN->completelyProcessed=parseCommand(sidrIN, (pCommandDataPtr)sidrIN->messageData);
			break;

		case pDNSLookupMessage:
			CheckDNSResponse(sidrIN, (pDNSLookupPtr)sidrIN->messageData);
			break;

		case pTextConnectionMessage:
			handleConnection(sidrIN, (pTextConnectionDataPtr)sidrIN->messageData);
		
		case pIPCMessage:
			IPC(sidrIN, (pIPCMessageDataPtr)sidrIN->messageData);
	}
	#if !__POWERPC__
	ExitCodeResource();
	#endif
}