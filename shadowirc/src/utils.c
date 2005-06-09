/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2005 John Bafford
	dshadow@shadowirc.com
	http://www.shadowirc.com

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <Carbon/Carbon.h>

#include "stringlist.h"
#include "Inline.h"
#include "IRCGlobals.h"
#include "Floaters.h"
#include "utils.h"
#include "ApplBase.h"

inline char inupc(char c);
static char matchFrom(ConstStr255Param s, ConstStr255Param mask, int i,  int j);


pascal void SecsToHMS(long l, LongString *ls)
{
	int d, h, m, s;
	Str255 hs;
	
	d = l / 86400;
	h = (l%86400) / 3600;
	m = (l%3600)/60;
	s=l%60;
	
	ls->len=0;
	if(d)
	{
		NumToString(d, hs);
		LSConcatStrAndStr(hs, "\p days, ", ls);
	}
	
	if(h)
	{
		NumToString(h, hs);
		LSConcatLSAndStrAndStr(ls, hs, "\p hours, ", ls);
	}

	if(m)
	{
		NumToString(m, hs);
		LSConcatLSAndStrAndStr(ls, hs, "\p minutes, ", ls);
	}
	
	if(s || (h==0 && m==0 && d==0))
	{
		NumToString(s, hs);
		LSConcatLSAndStrAndStr(ls, hs, "\p seconds, ", ls);
	}
	
	ls->len-=2;
	LSMakeStr(*ls);
}

pascal ControlHandle GetControlHandle(DialogPtr theDialog, short theItem)
{
	ControlHandle theControl = 0;
	
	GetDialogItemAsControl(theDialog, theItem, &theControl);
	return theControl;
}

pascal void GetText(DialogPtr theDialog, short itemNum, StringPtr theText)
{
	short			iType;
	Handle			iHandle;
	Rect			iRect;
	ControlHandle theControl;
	
	theText[0] = 0;
	if(GetDialogItemAsControl(theDialog, itemNum, &theControl) == noErr)
	{
		Size theDataSize = 255;
		
		GetControlData(theControl, kControlEditTextPart, kControlEditTextTextTag, theDataSize, (Ptr) &theText[1], &theDataSize);
		
		theText[0] = (char)theDataSize;
	}
	else
	{
		GetDialogItem(theDialog, itemNum, &iType, &iHandle, &iRect);
		GetDialogItemText(iHandle, theText);
	}
}

pascal void SetText(DialogPtr theDialog, short itemNum, ConstStr255Param theText)
{
	short			iType;
	Handle			iHandle;
	Rect			iRect;
	ControlHandle	theControl;
	
	if(GetDialogItemAsControl(theDialog, itemNum, &theControl) == noErr)
	{
		SetControlData(theControl, kControlEditTextPart, kControlEditTextTextTag, theText[0], (Ptr)&theText[1]);
		DrawOneControl(theControl);
	}
	else
	{
		GetDialogItem(theDialog, itemNum, &iType, &iHandle, &iRect);
		SetDialogItemText(iHandle, theText);
		InvalWindowRect(GetDialogWindow(theDialog), &iRect);
	}
}

pascal char getCheckBox(DialogPtr d, short item)
{
	return GetControlValue(GetControlHandle(d,item))==1;
}

pascal void setButtonEnable(DialogPtr d, short i, char on)
{
	HiliteControl(GetControlHandle(d,i), (on)?0:254); //this isn't faster, but generates tighter code
}

pascal void setCheckBox(DialogPtr theDialog, short item, char up)
{
	SetControlValue(GetControlHandle(theDialog, item), up);
}

pascal char getFirstSelectedCell(ListHandle l, Cell *c)
{
	*(long*)c = 0;
	return LGetSelect(1, c, l);
}

pascal void makeCellVisible(ListHandle theList, Cell theCell)
{
	Rect visibleRect;
	short dCols, dRows;
	
	visibleRect=(**theList).visible;
	if(!PtInRect(theCell, &visibleRect))
	{
		if(theCell.h>visibleRect.right-1)
			dCols=theCell.h-visibleRect.right+1;
		else if(theCell.h < visibleRect.left)
			dCols=theCell.h-visibleRect.left;
		
		if(theCell.v > visibleRect.bottom -1)
			dRows=theCell.v-visibleRect.bottom+1;
		else if(theCell.v < visibleRect.top)
			dRows=theCell.v-visibleRect.top;
		LScroll(dCols, dRows, theList);
	}
}

pascal void selectOneCell(ListHandle l, Cell c)
{
	Cell nextSelectedCell;
	char moreCellsInList;
	
	if(getFirstSelectedCell(l, &nextSelectedCell))
	{
		while(LGetSelect(1, &nextSelectedCell, l))
			if((nextSelectedCell.h != c.h) || (nextSelectedCell.v != c.v))
				LSetSelect(0, nextSelectedCell, l);
			else
				moreCellsInList= LNextCell(1, 1, &nextSelectedCell, l);
	}
	
	LSetSelect(1, c, l);
}

pascal void drawListBorder(ListHandle myList)
{
	Rect myBorder;
	PenState myPenState;
	
	myBorder=(**myList).rView;
	GetPenState(&myPenState);
	PenSize(1,1);
	InsetRect(&myBorder, -1, -1);
	FrameRect(&myBorder);
	SetPenState(&myPenState);
}

pascal void padEnd(ConstStr255Param s, short num, StringPtr out)
{
	int x,y;
	
	if(s!=out)
		pstrcpy(s,out);
	x=num-out[0];
	if(x>0)
	{
		for(y=out[0]+1; y<=out[0]+1+x;y++)
			out[y]=' ';
		out[0]+=x;
	}
}

pascal void padBegin(ConstStr255Param s, short num, StringPtr out)
{
	int x=num-s[0];
	int i;
	if(x>0)
	{
		BlockMoveData(&s[1],&out[x+1],s[0]);
		for(i=1;i<=x;i++)
			out[i]=' ';
		out[0]=s[0]+x;
	}
	else
	{
		if(out!=s)
			BlockMoveData(s,out,s[0]+1);
	}
}

pascal void makeMask(StringPtr uah, StringPtr mask)
{
	char isIP;
	int x, p;
	Str255 user, host, st;
	
	x=0;
	
	p= pos('!', uah);
	if(p)
		pdelete(uah, 1, p);
	p= pos('@', uah);
	if(!p)
	{
		mask[0]=0;
		return;
	}
	myCopy(uah, 1, p, user); //user@
	myCopy(uah, p + 1, 255, host); //host
//fix identd
	p= pos('~', user);
	if(p)
		pdelete(user, 1, 1);
	
	pstrcpy(user, &user[3]);
	user[1]='*';
	user[2] = '!';
	user[3] = '*';
	user[0]+=3;
	if(user[3]==user[4]) //[3]=[4]=*
		pdelete(user, 3, 1);
	
	//check for IP
	x=0;
	if(countChar('.', host)==3)
	{
		isIP=true;
		do
		{
			x++;
			if(((host[x]<'0') || (host[x]>'9')) && (host[x]!='.'))
			{
				isIP=false;
				break;
			}
		} while(x<host[0]);
	}
	else
		isIP=false;
	
	p=revPos('.', host);
	if(isIP)
	{
		host[p]='x';
		p=revPos('.', host);
		*(long*)&host[p+1]='*.*_';
		host[0]=p+3;
		pstrcpy(host, &mask[user[0]]);
		pstrcpy(user, mask);
		mask[0]+=host[0];
	}
	else
	{
		pstrcpy(host, st);
		st[0]=p-1;
		p=revPos('.', st);
		if(p)
			pdelete(host, 1, p-1);
		p=user[0]+1;
		pstrcpy(user, mask);
		pstrcpy(host, &mask[p]);
		mask[0]+=mask[p]+1;
		mask[p]='*';
	}
}

#pragma mark -


pascal void pstrcpymax(ConstStringPtr src, StringPtr dest, char max)
{
	int x =  ((max>src[0])?src[0]:max);
	BlockMoveData(src, dest, x+1);
	dest[0] = x;
}

pascal void myCopy(ConstStr255Param s, short first, short last, StringPtr out)
{
	unsigned char len, final;

	if(first>last)
		out[0]=0;
	else
	{
		if(last>s[0])
			final=s[0];
		else
			final=last;
		
		len=final-first+1;
		if((s!=out)||(first!=1)) //If out is s, and first = 1, we're truncating.
			BlockMoveData(&s[first], &out[1], len);
		out[0]=len;
	}
}

pascal void pstrcat(ConstStr255Param src1, ConstStr255Param src2, StringPtr des)
{
	int x, cnt;
	
	x = src1[0];
	cnt = 0;
	
	if(src1 != des) //then we need to copy src1
	{
		if(src2 == des) //then the destination content needs to be moved down by src1 length
		{
			cnt = 255 - x;
			
			if(cnt > src2[0])
				cnt= src2[0];
			
			BlockMoveData(src2, &des[x], cnt + 1);
		}
		
		BlockMoveData(src1, des, x + 1);
	}
	
	//src1 is in proper place
	if(src2 != des) //then src2 needs to be copied too
	{
		cnt = 255 - x;
		
		if(cnt > src2[0])
			cnt = src2[0];
		BlockMoveData(&src2[1], &des[x] + 1, cnt);
	}
	
	des[0] = x + cnt;
}

pascal void pinsert(ConstStr255Param ins, StringPtr s, short insat)
{
	LongString ls, ls2;
	
	if(insat>s[0])
	{
		LSConcatStrAndStr(s, ins, &ls);
	}
	else if(insat<=1)
	{
		LSConcatStrAndStr(ins, s, &ls);
	}
	else
	{
		LSStrLS(s, &ls);
		LSCopy(&ls, insat+1, maxLSlen, &ls2);
		ls.len=insat;
		LSConcatLSAndStrAndLS(&ls, ins, &ls2, &ls);
	}
	LSCopyString(&ls, 1, 255, s);
}

pascal void pdelete(StringPtr s, short start, short len)
{
	if((len>=1) && (start<=s[0]))
	{
		short to;
		
		if((start+len-1)>s[0])
			to=s[0];
		else
			to=start+len-1;
		
		BlockMoveData(&s[start+len], &s[start], s[0]-to);
		s[0]-=len;
	}
}

pascal char pstrcmp(ConstStr255Param s1, ConstStr255Param s2)
{
	int x=s1[0];
	int z;
	
	for(z=0;;z++)
		if(s1[z]!=s2[z]) // loop through until we find a mismatch, and return false. this includes s[0]
			return 0;
		else
			if(z==x) // else if we're at strlen, return 1. we know the strings are equal len b/c that was 1st comp
				return 1;
	return 0; //getting here is an error. return zero!
}

inline char inupc(char c)
{
	if((c>='a') && (c<='z'))
		c-=32;
	return c;
}

pascal char pstrcasecmp(ConstStr255Param s1, ConstStr255Param s2)
{
	int x, z;
	
	if((x = s1[0]) != s2[0])
		return 0;
	
	for(z = 1; z <= x; z++)
		if(inupc(s1[z]) != inupc(s2[z]))
			return 0;
	
	return 1;
}

pascal char pstrcasecmp2(ConstStr255Param s1, ConstStr255Param s2)
{
	int x, z;
	
	if((x = s1[0]) != s2[0])
		return 0;
	
	for(z = 1; z <= x; z++)
		if(s1[z] != inupc(s2[z]))
			return 0;
	
	return 1;
}

pascal OSErr ParamString(StringPtr str, ConstStr255Param r0, ConstStr255Param r1, ConstStr255Param r2, ConstStr255Param r3)
{
	LongString ls;
	
	LSParamString(&ls, str, r0, r1, r2, r3);
	LSCopyString(&ls, 1, 255, str);
	return 0;
}
/*
	OSErr err;
	Handle hdl;
	short len;

	err = PtrToHand(str+1, &hdl, *str);
	if (!err)
	{
		if (r0)  Munger(hdl, 0, "^0", 2, r0+1, *r0);
		if (r1)  Munger(hdl, 0, "^1", 2, r1+1, *r1);
		if (r2)  Munger(hdl, 0, "^2", 2, r2+1, *r2);
		if (r3)  Munger(hdl, 0, "^3", 2, r3+1, *r3);
		len = GetHandleSize(hdl);
		if (len > 255)
			len = 255;
		*str = len;
		BlockMove(*hdl, str+1, len);
		DisposeHandle(hdl);
	}
	return err;
}
*/

pascal void LightenColor(RGBColor *rgb)
{
	HSLColor hsl;
	long l;
	long avg;
	
	RGB2HSL(rgb, &hsl);
	avg = (rgb->red + rgb->green + rgb->blue)/3;
	l = hsl.lightness + (65536 * (1- (float)((float)(avg)/(float)65535)) *0.55);
	if(l > 65535)
		l=65535;
	hsl.lightness=l;
	
	HSL2RGB(&hsl, rgb);
}

void SetupModalDialog(DialogPtr d, short okItem, short cancelItem)
{
	SetDialogDefaultItem(d, okItem);
	SetDialogCancelItem(d, cancelItem);
	SetDialogTracksCursor(d, true);
	if(!IsWindowVisible(GetDialogWindow(d)))
		ShowWindow(GetDialogWindow(d));
}

pascal void FinishModalDialog(void)
{
	SetThemeCursor(kThemeArrowCursor);
}

pascal unsigned char StandardDialogFilter(DialogPtr d, EventRecord *e, short *item)
{
	if(e->what == updateEvt)
	{
		*item = 0;
	}
	else if(e->what == nullEvent)
	{
		*item=0;
	}
	else if(e->what == mouseDown)
	{
		short i;
		long l;
		char c;
		WindowPtr p;
		
		i = FindWindow(e->where, &p);
		if(i == inMenuBar)
		{
			l = MenuSelect(e->where);
			if(*(short*)&l == EditMenu)
			{
				i = l & 0xFFFF;
				if(i >= 3 && i <= 7)
				{
					e->what = keyDown;
					e->modifiers = cmdKey;
					if(i == 3)
						c = 'x';
					else if(i==4)
						c = 'c';
					else if(i == 5)
						c = 'v';
					else if(i == 7)
						c = 'a';
					
					e->message = (e->message & 0xFFFFFF00) | c;
				}
			}
			else
				e->what = nullEvent;
			HiliteMenu(0);
		}
	}
	
	return StdFilterProc(d, e, item);
}

pascal void SetDlogFont(DialogPtr d)
{
	GrafPtr p;
	short fontNum;
	
	GetPort(&p);
	SetPortWindowPort(GetDialogWindow(d));
	GetFNum("\pGeneva", &fontNum);
	TextFont(fontNum);
	TextSize(10);
	SetPort(p);
}

pascal char upc(unsigned char c)
{
	if((c>='a') && (c<='z'))
		c-=32;
	return c;
}

pascal void ucase(StringPtr str)
{
	int x;
	int n = str[0];
	
	for(x=1;x<=n;x++)
		if((str[x]>='a') && (str[x]<='z'))
			str[x]-=32;
}

pascal void pstrcpyucase(ConstStr255Param src, Str255 dest)
{
	int x;
	int n = src[0];
	
	dest[0]=src[0];
	for(x=1;x<=n;x++)
		dest[x]=inupc(src[x]);
}

pascal unsigned short countChar(short c, const ConstStr255Param s)
{
	int x;
	unsigned short count=0;
	int n = s[0];
	
	for(x=1;x<=n;x++)
		if(s[x]==(char)c)
			count++;

	return count;
}

pascal unsigned short revPos(short c, ConstStr255Param s)
{
	int x=s[0];
	
	for(;x>0;x--)
		if(s[x]==(char)c)	
			return x;
	
	return 0;
}

pascal char isNumber(ConstStr255Param s)
{
	int n=s[0], x;
	
	for(x=1;x<=n;x++)
		if((s[x]<'0')||(s[x]>'9'))
			return false;
	
	return true;
}

pascal long IPStringToLong(ConstStr255Param s)
{
	short p;
	Str255 str, s2;
	long l;
	char ip[4];
	
	pstrcpy(s, str);
	
	p=pos('.', str);
	myCopy(str, 1, p-1, s2);
	pdelete(str, 1, p);
	StringToNum(s2, &l);
	ip[0]=l;
	
	p=pos('.', str);
	myCopy(str, 1, p-1, s2);
	pdelete(str, 1, p);
	StringToNum(s2, &l);
	ip[1]=l;
	
	p=pos('.', str);
	myCopy(str, 1, p-1, s2);
	pdelete(str, 1, p);
	StringToNum(s2, &l);
	ip[2]=l;
	
	StringToNum(str, &l);
	ip[3]=l;
	
	return *(long*)ip;
}

pascal char isIPNumber(ConstStr255Param s)
{
	int n=s[0], x;
	
	for(x=1;x<=n;x++)
		if((s[x]!='.') && ((s[x]<'0') || (s[x]>'9')))
			return false;
	
	return((n>=7)&&(n<=15));
}

pascal unsigned short pos(short c, ConstStr255Param s)
{
	int x;
	int n = s[0];
	
	for(x=1;x<=n;x++)
		if((char)c==s[x])
			return x;

	return  0;
}

/*
char MaskMatchC(const char* s, const char* mask)
{
	do
	{
		if(!*mask && !*s)
			return true;

		if(*mask=='*')
		{
			if(!*(++mask))
				return true;

			do
			{
				while(inupc(*s)!=inupc(*mask))
					if(!*(++s))
						return false;

				if(matchFrom(s,mask))
					return true;
			}while(*(++s));

			return false;
		}
		else if(*mask !='?')
			if(inupc(*mask) != inupc(*s))
				return false;

		mask++;
		s++;
	}while(1);
}
*/

static char matchFrom(ConstStr255Param s, ConstStr255Param mask, int i,  int j)
{
	do
	{
		if(i >= mask[0]  && j >= s[0])
			return true;
		
		if(mask[i]=='*')
		{
			i++;
			if(i>=mask[0])
				return true;
			
			do
			{
				while(inupc(s[j])!=inupc(mask[i]))
					if(!s[++j])
						return false;

				if(matchFrom(s,mask,i,j))
					return true;
			}while(s[++j]);
		
			return false;
		}
		else if(mask[i] !='?')
			if(inupc(mask[i]) != inupc(s[j]))
				return false;
		
		i++;
		j++;
	}while(1);
}

pascal char maskMatch(ConstStr255Param s, ConstStr255Param mask)
{
	return matchFrom(s,mask,1,1);
}

pascal void ulong64val(ConstStr255Param s, unsigned long long *out)
{
	unsigned long long l = 0;
	int i=1;

	while(i<=s[0])
		l = (l*10) + (s[i++]-'0');

	*out=l;
}

pascal ConstStringPtr GetIntStringPtr(STRnPtr list, short elt)
{
	ConstStringPtr sp;

	if(elt>list->num)
		return "\p";
	
	sp = list->list;
	while(--elt)
		sp += *sp + 1;	
	
	return sp;
}

pascal void GetIntString(StringPtr s, STRnPtr list, short elt)
{
	ConstStringPtr sp, max;

	if(elt>list->num)
	{
		s[0] = 0;
		return;
	}
		
	sp = list->list;
	while(--elt)
		sp += *sp + 1;	
	
	max = sp + *sp+ 1;
	while(sp < max)
	{
		*(long*)s = *(long*)sp;
		s += 4;
		sp += 4;
	}
}

pascal void LSGetIntString(LongString *ls, STRnPtr list, short elt)
{
	GetIntString(ls->data, list, elt);
	ls->len = ls->data[0];
}

pascal OSErr CopyResource(ResType type, short id, short source, short dest)
{
	short saveFile;
	Handle sourceResourceHdl;
	ResType ignoredType;
	short ignoredID;
	Str255 resourceName;
	short resAttributes;
	OSErr err;

	saveFile = CurResFile();
	UseResFile(source);

	sourceResourceHdl = Get1Resource(type, id);

	if(sourceResourceHdl)
	{
		GetResInfo(sourceResourceHdl, &ignoredID, &ignoredType, resourceName);
		resAttributes = GetResAttrs(sourceResourceHdl);
		DetachResource(sourceResourceHdl);
		UseResFile(dest);
		if(ResError() == noErr)
			AddResource(sourceResourceHdl, type, id, resourceName);
		if(ResError() == noErr)
			SetResAttrs(sourceResourceHdl, resAttributes);
		if(ResError() == noErr)
			ChangedResource(sourceResourceHdl);
		if(ResError() == noErr)
			WriteResource(sourceResourceHdl);
		}

	err = ResError();

	ReleaseResource(sourceResourceHdl);
	UseResFile(saveFile);
	
	UpdateResFile(dest);
	return err;
}

pascal void DeleteResourceType(short resFile, ResType type)
{
	Handle h;
	if(resFile)
	{
		short rf = CurResFile();
		SetResLoad(false);
		
		while((h = Get1IndResource(type, 1)) != 0)
			RemoveResource(h);
		
		UpdateResFile(resFile);
		
		SetResLoad(true);
		UseResFile(rf);
	}
}

pascal void NextArg(StringPtr from, StringPtr arg)
{
	short i = pos(' ',from);
	if(!i)
	{
		if(arg)
			pstrcpy(from,arg);
		from[0]=0;
	}
	else
	{
		if(arg)
			myCopy(from,1,i-1, arg);
		while(i<from[0])
		{
			if((from[i+1]!=':') && (from[i+1]!=' '))
				break;
			i++;
		}
		pdelete(from,1,i);
	}
}

pascal StringPtr NewPString(const unsigned char *pstr)
{
	StringPtr str;
	
	if(pstr)
	{
		str = (StringPtr)NewPtr(pstr[0] + 1);
		pstrcpy(pstr, str);
	}
	else
		str = 0;
	
	return str;
}

void * safe_malloc (size_t size)
{
        void *result;
        result = malloc(size);
        if (result == NULL) {
                /* XXX landonf: fatal() */
                exit(-1);
        }
        return (result);
}