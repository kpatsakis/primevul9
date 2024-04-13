XkbSizeExplicit(XkbDescPtr xkb,xkbGetMapReply *rep)
{
    unsigned	i,len,nRtrn;

    if (((rep->present&XkbExplicitComponentsMask)==0)||(rep->nKeyExplicit<1)||
	(!xkb)||(!xkb->server)||(!xkb->server->explicit)) {
	rep->present&= ~XkbExplicitComponentsMask;
	rep->firstKeyExplicit= rep->nKeyExplicit= 0;
	rep->totalKeyExplicit= 0;
	return 0;
    }
    for (nRtrn=i=0;i<rep->nKeyExplicit;i++) {
	if (xkb->server->explicit[i+rep->firstKeyExplicit]!=0)
	    nRtrn++;
    }
    rep->totalKeyExplicit= nRtrn;
    len= XkbPaddedSize(nRtrn*2); /* two bytes per non-zero explicit component */
    return len;
}