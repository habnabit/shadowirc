/*
 *	WELongCoords.c
 *
 *	WASTE PROJECT
 *  Long Coordinates
 *
 *  Copyright (c) 1993-1998 Marco Piovanelli
 *	All Rights Reserved
 *
 *  C port by Dan Crevier
 *
 */


#include "WASTEIntf.h"

enum
{
	kQDMin = -32767L,
	kQDMax = +32767L
};

pascal SInt32 _WEPinInRange(SInt32 value, SInt32 rangeStart, SInt32 rangeEnd)
{
	return ((value > rangeEnd) ? rangeEnd : ((value < rangeStart) ? rangeStart : value));
}

pascal void WELongPointToPoint(const LongPt *lp, Point *p)
{
	p->v = _WEPinInRange(lp->v, kQDMin, kQDMax);
	p->h = _WEPinInRange(lp->h, kQDMin, kQDMax);
}

pascal void WEPointToLongPoint(Point p, LongPt *lp)
{
	lp->v = p.v;
	lp->h = p.h;
}

pascal void WESetLongRect(LongRect *lr, SInt32 left, SInt32 top, SInt32 right, SInt32 bottom)
{
	lr->top    = top;
	lr->left   = left;
	lr->bottom = bottom;
	lr->right  = right;
}

pascal void WELongRectToRect(const LongRect *lr, Rect *r)
{
	WELongPointToPoint((const LongPt *) lr, (Point *) r);
	WELongPointToPoint((const LongPt *) lr + 1, (Point *) r + 1);
}

pascal void WERectToLongRect(const Rect *r, LongRect *lr)
{
	lr->top    = r->top;
	lr->left   = r->left;
	lr->bottom = r->bottom;
	lr->right  = r->right;
}

pascal void WEOffsetLongRect(LongRect *lr, SInt32 hOffset, SInt32 vOffset)
{
	lr->top    += vOffset;
	lr->left   += hOffset;
	lr->bottom += vOffset;
	lr->right  += hOffset;
}

pascal Boolean WELongPointInLongRect(const LongPt *lp, const LongRect *lr)
{
	return ((lp->v >= lr->top) && (lp->h >= lr->left) && (lp->v < lr->bottom) && (lp->h < lr->right));
}
