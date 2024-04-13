XkbWriteKeyActions(XkbDescPtr xkb,xkbGetMapReply *rep,char *buf,
							ClientPtr client)
{
    unsigned		i;
    CARD8 *		numDesc;
    XkbAnyAction *	actDesc;

    numDesc = (CARD8 *)buf;
    for (i=0;i<rep->nKeyActs;i++) {
	if (xkb->server->key_acts[i+rep->firstKeyAct]==0)
	     numDesc[i] = 0;
	else numDesc[i] = XkbKeyNumActions(xkb,(i+rep->firstKeyAct));
    }
    buf+= XkbPaddedSize(rep->nKeyActs);

    actDesc = (XkbAnyAction *)buf;
    for (i=0;i<rep->nKeyActs;i++) {
	if (xkb->server->key_acts[i+rep->firstKeyAct]!=0) {
	    unsigned int num;
	    num = XkbKeyNumActions(xkb,(i+rep->firstKeyAct));
	    memcpy((char *)actDesc,
		   (char*)XkbKeyActionsPtr(xkb,(i+rep->firstKeyAct)),
		   num*SIZEOF(xkbActionWireDesc));
	    actDesc+= num;
	}
    }
    buf = (char *)actDesc;
    return buf;
}