CheckKeyExplicit(	XkbDescPtr	xkb,
			xkbSetMapReq *	req,
			CARD8 **	wireRtrn,
			int	*	errorRtrn)
{
register CARD8 *	wire = *wireRtrn;
CARD8	*		start;
register unsigned 	i;
int			first,last;

    if (((req->present&XkbExplicitComponentsMask)==0)||(req->nKeyExplicit<1)) {
	req->present&= ~XkbExplicitComponentsMask;
	req->nKeyExplicit= 0;
	return 1;
    }
    first= req->firstKeyExplicit;
    last=  first+req->nKeyExplicit-1;
    if (first<req->minKeyCode) {
	*errorRtrn = _XkbErrCode3(0x51,first,req->minKeyCode);
	return 0;
    }
    if (last>req->maxKeyCode) {
	*errorRtrn = _XkbErrCode3(0x52,last,req->maxKeyCode);
	return 0;
    }
    start= wire; 
    for (i=0;i<req->totalKeyExplicit;i++,wire+=2) {
	if ((wire[0]<first)||(wire[0]>last)) {
	    *errorRtrn = _XkbErrCode4(0x53,first,last,wire[0]);
	    return 0;
	}
	if (wire[1]&(~XkbAllExplicitMask)) {
	     *errorRtrn= _XkbErrCode3(0x52,~XkbAllExplicitMask,wire[1]);
	     return 0;
	}
    }
    wire+= XkbPaddedSize(wire-start)-(wire-start);
    *wireRtrn= wire;
    return 1;
}