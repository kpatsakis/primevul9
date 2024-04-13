XkbSizeModifierMap(XkbDescPtr xkb,xkbGetMapReply *rep)
{
    unsigned	i,len,nRtrn;

    if (((rep->present&XkbModifierMapMask)==0)||(rep->nModMapKeys<1)||
	(!xkb)||(!xkb->map)||(!xkb->map->modmap)) {
	rep->present&= ~XkbModifierMapMask;
	rep->firstModMapKey= rep->nModMapKeys= 0;
	rep->totalModMapKeys= 0;
	return 0;
    }
    for (nRtrn=i=0;i<rep->nModMapKeys;i++) {
	if (xkb->map->modmap[i+rep->firstModMapKey]!=0)
	    nRtrn++;
    }
    rep->totalModMapKeys= nRtrn;
    len= XkbPaddedSize(nRtrn*2); /* two bytes per non-zero modmap component */
    return len;
}