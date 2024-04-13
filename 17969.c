SetKeyBehaviors(	XkbSrvInfoPtr	 xkbi,
    			xkbSetMapReq	*req,
    			xkbBehaviorWireDesc	*wire,
    			XkbChangesPtr	 changes)
{
register unsigned i;
int maxRG = -1;
XkbDescPtr       xkb = xkbi->desc;
XkbServerMapPtr	 server = xkb->server;
unsigned	 first,last;

    first= req->firstKeyBehavior;
    last= req->firstKeyBehavior+req->nKeyBehaviors-1;
    bzero(&server->behaviors[first],req->nKeyBehaviors*sizeof(XkbBehavior));
    for (i=0;i<req->totalKeyBehaviors;i++) {
	if ((server->behaviors[wire->key].type&XkbKB_Permanent)==0) {
	    server->behaviors[wire->key].type= wire->type;
	    server->behaviors[wire->key].data= wire->data;
	    if ((wire->type==XkbKB_RadioGroup)&&(((int)wire->data)>maxRG))
		maxRG= wire->data + 1;
	}
	wire++;
    }

    if (maxRG>(int)xkbi->nRadioGroups) {
        int sz = maxRG*sizeof(XkbRadioGroupRec);
        if (xkbi->radioGroups)
             xkbi->radioGroups=(XkbRadioGroupPtr)_XkbRealloc(xkbi->radioGroups,sz);
        else xkbi->radioGroups= (XkbRadioGroupPtr)_XkbCalloc(1, sz);
        if (xkbi->radioGroups) {
             if (xkbi->nRadioGroups)
                bzero(&xkbi->radioGroups[xkbi->nRadioGroups],
                        (maxRG-xkbi->nRadioGroups)*sizeof(XkbRadioGroupRec));
             xkbi->nRadioGroups= maxRG;
        }
        else xkbi->nRadioGroups= 0;
        /* should compute members here */
    }
    if (changes->map.changed&XkbKeyBehaviorsMask) {
	unsigned oldLast;
	oldLast= changes->map.first_key_behavior+
					changes->map.num_key_behaviors-1;
        if (changes->map.first_key_behavior<req->firstKeyBehavior)
             first= changes->map.first_key_behavior;
        if (oldLast>last)
            last= oldLast;
    }
    changes->map.changed|= XkbKeyBehaviorsMask;
    changes->map.first_key_behavior = first;
    changes->map.num_key_behaviors = (last-first+1);
    return (char *)wire;
}