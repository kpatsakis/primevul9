XkbSizeKeyBehaviors(XkbDescPtr xkb,xkbGetMapReply *rep)
{
    unsigned		i,len,nBhvr;
    XkbBehavior *	bhv;

    if (((rep->present&XkbKeyBehaviorsMask)==0)||(rep->nKeyBehaviors<1)||
	(!xkb)||(!xkb->server)||(!xkb->server->behaviors)) {
	rep->present&= ~XkbKeyBehaviorsMask;
	rep->firstKeyBehavior= rep->nKeyBehaviors= 0;
	rep->totalKeyBehaviors= 0;
	return 0;
    }
    bhv= &xkb->server->behaviors[rep->firstKeyBehavior];
    for (nBhvr=i=0;i<rep->nKeyBehaviors;i++,bhv++) {
	if (bhv->type!=XkbKB_Default)
	    nBhvr++;
    }
    len= nBhvr*SIZEOF(xkbBehaviorWireDesc);
    rep->totalKeyBehaviors= nBhvr;
    return len;
}