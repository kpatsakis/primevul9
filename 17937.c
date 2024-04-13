CheckModifierMap(XkbDescPtr xkb,xkbSetMapReq *req,CARD8 **wireRtrn,int *errRtrn)
{
register CARD8 *	wire = *wireRtrn;
CARD8	*		start;
register unsigned 	i;
int			first,last;

    if (((req->present&XkbModifierMapMask)==0)||(req->nModMapKeys<1)) {
	req->present&= ~XkbModifierMapMask;
	req->nModMapKeys= 0;
	return 1;
    }
    first= req->firstModMapKey;
    last=  first+req->nModMapKeys-1;
    if (first<req->minKeyCode) {
	*errRtrn = _XkbErrCode3(0x61,first,req->minKeyCode);
	return 0;
    }
    if (last>req->maxKeyCode) {
	*errRtrn = _XkbErrCode3(0x62,last,req->maxKeyCode);
	return 0;
    }
    start= wire; 
    for (i=0;i<req->totalModMapKeys;i++,wire+=2) {
	if ((wire[0]<first)||(wire[0]>last)) {
	    *errRtrn = _XkbErrCode4(0x63,first,last,wire[0]);
	    return 0;
	}
    }
    wire+= XkbPaddedSize(wire-start)-(wire-start);
    *wireRtrn= wire;
    return 1;
}