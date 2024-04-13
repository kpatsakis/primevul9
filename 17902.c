CheckKeyBehaviors(	XkbDescPtr 		xkb,
			xkbSetMapReq *		req,
			xkbBehaviorWireDesc **	wireRtrn,
			int *			errorRtrn)
{
register xkbBehaviorWireDesc *	wire = *wireRtrn;
register XkbServerMapPtr	server = xkb->server;
register unsigned	 	i;
unsigned			first,last;

    if (((req->present&XkbKeyBehaviorsMask)==0)||(req->nKeyBehaviors<1)) {
	req->present&= ~XkbKeyBehaviorsMask;
	req->nKeyBehaviors= 0;
	return 1;
    }
    first= req->firstKeyBehavior;
    last=  req->firstKeyBehavior+req->nKeyBehaviors-1;
    if (first<req->minKeyCode) {
	*errorRtrn = _XkbErrCode3(0x31,first,req->minKeyCode);
	return 0;
    }
    if (last>req->maxKeyCode) {
	*errorRtrn = _XkbErrCode3(0x32,last,req->maxKeyCode);
	return 0;
    }
	
    for (i=0;i<req->totalKeyBehaviors;i++,wire++) {
	if ((wire->key<first)||(wire->key>last)) {
	    *errorRtrn = _XkbErrCode4(0x33,first,last,wire->key);
	    return 0;
	}
	if ((wire->type&XkbKB_Permanent)&&
	    ((server->behaviors[wire->key].type!=wire->type)||
	     (server->behaviors[wire->key].data!=wire->data))) {
	    *errorRtrn = _XkbErrCode3(0x33,wire->key,wire->type);
	    return 0;
	}
	if ((wire->type==XkbKB_RadioGroup)&&
		((wire->data&(~XkbKB_RGAllowNone))>XkbMaxRadioGroups)) {
	    *errorRtrn= _XkbErrCode4(0x34,wire->key,wire->data,
							XkbMaxRadioGroups);
	    return 0;
	}
	if ((wire->type==XkbKB_Overlay1)||(wire->type==XkbKB_Overlay2)) {
	    CHK_KEY_RANGE2(0x35,wire->key,1,xkb,*errorRtrn,0);
	}
    }
    *wireRtrn = wire;
    return 1;
}