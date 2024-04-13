CheckVirtualModMap(	XkbDescPtr xkb,
			xkbSetMapReq *req,
			xkbVModMapWireDesc **wireRtrn,
			int *errRtrn)
{
register xkbVModMapWireDesc *	wire = *wireRtrn;
register unsigned 		i;
int				first,last;

    if (((req->present&XkbVirtualModMapMask)==0)||(req->nVModMapKeys<1)) {
	req->present&= ~XkbVirtualModMapMask;
	req->nVModMapKeys= 0;
	return 1;
    }
    first= req->firstVModMapKey;
    last=  first+req->nVModMapKeys-1;
    if (first<req->minKeyCode) {
	*errRtrn = _XkbErrCode3(0x71,first,req->minKeyCode);
	return 0;
    }
    if (last>req->maxKeyCode) {
	*errRtrn = _XkbErrCode3(0x72,last,req->maxKeyCode);
	return 0;
    }
    for (i=0;i<req->totalVModMapKeys;i++,wire++) {
	if ((wire->key<first)||(wire->key>last)) {
	    *errRtrn = _XkbErrCode4(0x73,first,last,wire->key);
	    return 0;
	}
    }
    *wireRtrn= wire;
    return 1;
}