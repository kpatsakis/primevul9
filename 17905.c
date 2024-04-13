XkbWriteKeyBehaviors(XkbDescPtr xkb,xkbGetMapReply *rep,char *buf,
							ClientPtr client)
{
    unsigned		i;
    xkbBehaviorWireDesc	*wire;
    XkbBehavior		*pBhvr;

    wire = (xkbBehaviorWireDesc *)buf;
    pBhvr= &xkb->server->behaviors[rep->firstKeyBehavior];
    for (i=0;i<rep->nKeyBehaviors;i++,pBhvr++) {
	if (pBhvr->type!=XkbKB_Default) {
	    wire->key=  i+rep->firstKeyBehavior;
	    wire->type= pBhvr->type;
	    wire->data= pBhvr->data;
	    wire++;
	}
    }
    buf = (char *)wire;
    return buf;
}