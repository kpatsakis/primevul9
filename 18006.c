SetKeySyms(	ClientPtr		client,
		XkbDescPtr		xkb,
		xkbSetMapReq *		req,
		xkbSymMapWireDesc *	wire,
		XkbChangesPtr 		changes,
		DeviceIntPtr		dev)
{
register unsigned 	i,s;
XkbSymMapPtr		oldMap;
KeySym *		newSyms;
KeySym *		pSyms;
unsigned		first,last;

    oldMap = &xkb->map->key_sym_map[req->firstKeySym];
    for (i=0;i<req->nKeySyms;i++,oldMap++) {
	pSyms = (KeySym *)&wire[1];
	if (wire->nSyms>0) {
	    newSyms = XkbResizeKeySyms(xkb,i+req->firstKeySym,wire->nSyms);
	    for (s=0;s<wire->nSyms;s++) {
		newSyms[s]= pSyms[s];
	    }
	    if (client->swapped) {
		int n;
		for (s=0;s<wire->nSyms;s++) {
		    swapl(&newSyms[s],n);
		}
	    }
	}
	oldMap->kt_index[0] = wire->ktIndex[0];
	oldMap->kt_index[1] = wire->ktIndex[1];
	oldMap->kt_index[2] = wire->ktIndex[2];
	oldMap->kt_index[3] = wire->ktIndex[3];
	oldMap->group_info = wire->groupInfo;
	oldMap->width = wire->width;
	wire= (xkbSymMapWireDesc *)&pSyms[wire->nSyms];
    }
    first= req->firstKeySym;
    last= first+req->nKeySyms-1;
    if (changes->map.changed&XkbKeySymsMask) {
	int oldLast= (changes->map.first_key_sym+changes->map.num_key_syms-1);
	if (changes->map.first_key_sym<first)
	    first= changes->map.first_key_sym;
	if (oldLast>last)
	    last= oldLast;
    }
    changes->map.changed|= XkbKeySymsMask;
    changes->map.first_key_sym = first;
    changes->map.num_key_syms = (last-first+1);

    s= 0;
    for (i=xkb->min_key_code;i<=xkb->max_key_code;i++) {
	if (XkbKeyNumGroups(xkb,i)>s)
	    s= XkbKeyNumGroups(xkb,i);
    }
    if (s!=xkb->ctrls->num_groups) {
	xkbControlsNotify	cn;
	XkbControlsRec		old;
	cn.keycode= 0;
	cn.eventType= 0;
	cn.requestMajor= XkbReqCode;
	cn.requestMinor= X_kbSetMap;
	old= *xkb->ctrls;
	xkb->ctrls->num_groups= s;
	if (XkbComputeControlsNotify(dev,&old,xkb->ctrls,&cn,False))
	    XkbSendControlsNotify(dev,&cn);
    }
    return (char *)wire;
}