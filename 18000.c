XkbSizeVirtualModMap(XkbDescPtr xkb,xkbGetMapReply *rep)
{
    unsigned	i,len,nRtrn;

    if (((rep->present&XkbVirtualModMapMask)==0)||(rep->nVModMapKeys<1)||
	(!xkb)||(!xkb->server)||(!xkb->server->vmodmap)) {
	rep->present&= ~XkbVirtualModMapMask;
	rep->firstVModMapKey= rep->nVModMapKeys= 0;
	rep->totalVModMapKeys= 0;
	return 0;
    }
    for (nRtrn=i=0;i<rep->nVModMapKeys-1;i++) {
	if (xkb->server->vmodmap[i+rep->firstVModMapKey]!=0)
	    nRtrn++;
    }
    rep->totalVModMapKeys= nRtrn;
    len= nRtrn*SIZEOF(xkbVModMapWireDesc);
    return len;
}