/*	numusers.c, © John Bafford 1998-1999
		ShadowIRC Sample Plugin
		Version 1.1
*/

/*	This plugin demonstrates the use of message window widgets.
*/

#include "ShadowIRC.h"

pascal void CheckNewWindow(ShadowIRCDataRecord* sidr, pMWNewData *p);
pascal void DrawWidget(ShadowIRCDataRecord* sidr, pMWWidgetUpdateData *p);

enum {
	kMyWidget = 'unum'
};

ShadowIRCDataRecord* sidr; //For 68k plugins, so we don't have to pass sidr all over the place in the PPC version for no reason at all.

pascal void CheckNewWindow(ShadowIRCDataRecord* sidr, pMWNewData *p)
{
	#pragma unused(sidr)
	if(p->mw->winType == chanWin) //if and only if it's a channel window
	{
		short wid;
		mwWidgetPtr thisWidget;
		
		//We don't need to use getport/setport pairs because we're guaranteed to have the message window as the current port
		TextSize(9);
		TextFace(0);
		wid=StringWidth("\p0/0/0/0") + 3;
		thisWidget = MWNewWidget(p->mw, kMyWidget, mwAlignRight, wid);
		thisWidget->data = 0;
	}
}

pascal void DrawWidget(ShadowIRCDataRecord* sidr, pMWWidgetUpdateData *p)
{
	#pragma unused(sidr)
	mwWidgetPtr w = p->widget;
	channelPtr ch;
	
	if(w->type == kMyWidget)
	{
		short users = 0, voice = 0, ops = 0, opers = 0;
		Str255 s, s2, s3, s4;
		short newwid;
		
		w->data = TickCount();
		//Draw our left border (because this is a right aligned widget
		MoveTo(w->drawArea.left, w->drawArea.top);
		LineTo(w->drawArea.left, w->drawArea.bottom);

		//Draw the text
		ch = MWGetChannel(w->mw);
		if(ch) //we do have a valid channel
		{
			UserListPtr u = ch->userlist;
			
			while(u)
			{
				users++;
				if(u->hasVoice)
					voice++;
				if(u->isOp)
					ops++;
				if(u->isOper)
					opers++;
				u=u->next;
			}
			
			NumToString(users, s);
			s[++s[0]]='/'; //tack on a divider
			NumToString(voice, s2);
			s2[++s2[0]]='/';
			NumToString(ops, s3);
			s3[++s3[0]]='/';
			NumToString(opers, s4);
			newwid = StringWidth(s) + StringWidth(s2) + StringWidth(s3) + StringWidth(s4) + 3;

			//Just to be spiffy, we're going to allow for dynamic resizing of the display.
			//We change our asked width, recalculate the widths of the widgets and redraw the status
			//area. We need to call DrawMWinStatus() so that the other widgets can redraw, since
			//they've also been resized.
			//There's just one slight little problem. Since DrawMWinStatus() is the function
			//that's calling you, you don't want two instances of DrawMWinStatus() trying to draw,
			//because you're not going to get correct results.
			//So, DrawMWinStatus() uses some evil voodoo (static variables and a goto) to take
			//care of that condition. What it means for you is that you **MUST** immediately
			//return and do *NOTHING* after calling DrawMWinStatus. You'll get called again
			//to draw. Ideally, you should bring the plugin back to the state before it
			//was called this time.
			//Note that this only applies if you're calling DrawMWinStatus() while you're being called
			//by it. This same discussion would apply to UpdateInputLine() and resizing inputline widgets.
			if(newwid != w->askedWidth)
			{
				w->askedWidth = newwid;
				MWRecalculateRects((MWPtr)w->mw);
				DrawMWinStatus((MWPtr)w->mw);
				return;
			}
			
			MoveTo(w->drawArea.left + 3, w->drawArea.bottom-3);
			DrawString(s);
			DrawString(s2);
			DrawString(s3);
			DrawString(s4);
		}
		else
		{
			MoveTo(w->drawArea.left + 3, w->drawArea.bottom-3);
			DrawString("\pNone");
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
			l=ShadowIRCVersion(0);
			if(l<0x0100004d) //we must have ShadowIRC 1.0d77 or later.
				((pVersionCheckDataPtr)sidrIN->messageData)->version = pVersionShadowIRCTooOld;
			else
				((pVersionCheckDataPtr)sidrIN->messageData)->version = pVersionCheckMessageReply;
			break;
			
		case pInitMessage:
			sidrIN->yourInfo->captureMessages[pMWNewMessage]=1;
			break;
		
		case pMWNewMessage:
			CheckNewWindow(sidr, (pMWNewDataPtr)sidr->messageData);
			break;
		
		case pMWWidgetUpdateMessage:
			DrawWidget(sidr, (pMWWidgetUpdateDataPtr)sidr->messageData);
			break;
			
		case pMWWidgetClickMessage: //redraw the display if we get a click
			DrawMWinStatus((MWPtr)(((pMWWidgetUpdateDataPtr)sidr->messageData)->widget->mw));
			break;

		case pMWWidgetDestroyMesage:
			break;
	}
	#if !__POWERPC__
	ExitCodeResource();
	#endif
}