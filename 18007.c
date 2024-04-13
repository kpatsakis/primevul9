_XkbSetMap(ClientPtr client, DeviceIntPtr dev, xkbSetMapReq *req, char *values)
{
    XkbEventCauseRec	cause;
    XkbChangesRec	change;
    Bool		sentNKN;
    XkbSrvInfoPtr       xkbi;
    XkbDescPtr          xkb;

    xkbi= dev->key->xkbInfo;
    xkb = xkbi->desc;

    XkbSetCauseXkbReq(&cause,X_kbSetMap,client);
    bzero(&change, sizeof(change));
    sentNKN = False;
    if ((xkb->min_key_code!=req->minKeyCode)||
        (xkb->max_key_code!=req->maxKeyCode)) {
	Status			status;
	xkbNewKeyboardNotify	nkn;
	nkn.deviceID = nkn.oldDeviceID = dev->id;
	nkn.oldMinKeyCode = xkb->min_key_code;
	nkn.oldMaxKeyCode = xkb->max_key_code;
	status= XkbChangeKeycodeRange(xkb, req->minKeyCode,
                                      req->maxKeyCode, &change);
	if (status != Success)
	    return status; /* oh-oh. what about the other keyboards? */
	nkn.minKeyCode = xkb->min_key_code;
	nkn.maxKeyCode = xkb->max_key_code;
	nkn.requestMajor = XkbReqCode;
	nkn.requestMinor = X_kbSetMap;
	nkn.changed = XkbNKN_KeycodesMask;
	XkbSendNewKeyboardNotify(dev,&nkn);
	sentNKN = True;
    }

    if (req->present&XkbKeyTypesMask) {
	values = SetKeyTypes(xkb,req,(xkbKeyTypeWireDesc *)values,&change);
	if (!values)	goto allocFailure;
    }
    if (req->present&XkbKeySymsMask) {
	values = SetKeySyms(client,xkb,req,(xkbSymMapWireDesc *)values,&change,dev);
	if (!values)	goto allocFailure;
    }
    if (req->present&XkbKeyActionsMask) {
	values = SetKeyActions(xkb,req,(CARD8 *)values,&change);
	if (!values)	goto allocFailure;
    }
    if (req->present&XkbKeyBehaviorsMask) {
	values= SetKeyBehaviors(xkbi,req,(xkbBehaviorWireDesc *)values,&change);
	if (!values)	goto allocFailure;
    }
    if (req->present&XkbVirtualModsMask)
	values= SetVirtualMods(xkbi,req,(CARD8 *)values,&change);
    if (req->present&XkbExplicitComponentsMask)
	values= SetKeyExplicit(xkbi,req,(CARD8 *)values,&change);
    if (req->present&XkbModifierMapMask)
	values= SetModifierMap(xkbi,req,(CARD8 *)values,&change);
    if (req->present&XkbVirtualModMapMask)
	values= SetVirtualModMap(xkbi,req,(xkbVModMapWireDesc *)values,&change);
    if (((values-((char *)req))/4)!=req->length) {
	ErrorF("[xkb] Internal error! Bad length in XkbSetMap (after set)\n");
	client->errorValue = values-((char *)&req[1]);
	return BadLength;
    }
    if (req->flags&XkbSetMapRecomputeActions) {
	KeyCode		first,last,firstMM,lastMM;
	if (change.map.num_key_syms>0) {
	    first= change.map.first_key_sym;
	    last= first+change.map.num_key_syms-1;
	}
	else first= last= 0;
	if (change.map.num_modmap_keys>0) {
	    firstMM= change.map.first_modmap_key;
	    lastMM= first+change.map.num_modmap_keys-1;
	}
	else firstMM= lastMM= 0;
	if ((last>0) && (lastMM>0)) {
	    if (firstMM<first)
		first= firstMM;
	    if (lastMM>last)
		last= lastMM;
	}
	else if (lastMM>0) {
	    first= firstMM;
	    last= lastMM;
	}
	if (last>0) {
	    unsigned check= 0;
	    XkbUpdateActions(dev,first,(last-first+1),&change,&check,&cause);
	    if (check)
		XkbCheckSecondaryEffects(xkbi,check,&change,&cause);
	}
    }
    if (!sentNKN)
	XkbSendNotification(dev,&change,&cause);

    XkbUpdateCoreDescription(dev,False);
    return Success;
allocFailure:
    return BadAlloc;
}