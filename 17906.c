SetKeyActions(	XkbDescPtr	xkb,
		xkbSetMapReq *	req,
		CARD8 *		wire,
		XkbChangesPtr	changes)
{
register unsigned	i,first,last;
CARD8 *			nActs = wire;
XkbAction *		newActs;
    
    wire+= XkbPaddedSize(req->nKeyActs);
    for (i=0;i<req->nKeyActs;i++) {
	if (nActs[i]==0)
	    xkb->server->key_acts[i+req->firstKeyAct]= 0;
	else {
	    newActs= XkbResizeKeyActions(xkb,i+req->firstKeyAct,nActs[i]);
	    memcpy((char *)newActs,(char *)wire,
					nActs[i]*SIZEOF(xkbActionWireDesc));
	    wire+= nActs[i]*SIZEOF(xkbActionWireDesc);
	}
    }
    first= req->firstKeyAct;
    last= (first+req->nKeyActs-1);
    if (changes->map.changed&XkbKeyActionsMask) {
	int oldLast;
	oldLast= changes->map.first_key_act+changes->map.num_key_acts-1;
	if (changes->map.first_key_act<first)
	    first= changes->map.first_key_act;
	if (oldLast>last)
	    last= oldLast;
    }
    changes->map.changed|= XkbKeyActionsMask;
    changes->map.first_key_act= first;
    changes->map.num_key_acts= (last-first+1);
    return (char *)wire;
}