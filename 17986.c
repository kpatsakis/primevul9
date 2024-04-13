XkbSizeKeyActions(XkbDescPtr xkb,xkbGetMapReply *rep)
{
    unsigned		i,len,nActs;
    register KeyCode	firstKey;

    if (((rep->present&XkbKeyActionsMask)==0)||(rep->nKeyActs<1)||
	(!xkb)||(!xkb->server)||(!xkb->server->key_acts)) {
	rep->present&= ~XkbKeyActionsMask;
	rep->firstKeyAct= rep->nKeyActs= 0;
	rep->totalActs= 0;
	return 0;
    }
    firstKey= rep->firstKeyAct;
    for (nActs=i=0;i<rep->nKeyActs;i++) {
	if (xkb->server->key_acts[i+firstKey]!=0)
	    nActs+= XkbKeyNumActions(xkb,i+firstKey);
    }
    len= XkbPaddedSize(rep->nKeyActs)+(nActs*SIZEOF(xkbActionWireDesc));
    rep->totalActs= nActs;
    return len;
}