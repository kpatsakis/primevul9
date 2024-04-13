_XkbSetMapChecks(ClientPtr client, DeviceIntPtr dev, xkbSetMapReq *req, char* values)
{
    XkbSrvInfoPtr       xkbi;
    XkbDescPtr          xkb;
    int                 error;
    int                 nTypes = 0, nActions;
    CARD8               mapWidths[XkbMaxLegalKeyCode + 1];
    CARD16              symsPerKey[XkbMaxLegalKeyCode + 1];

    xkbi= dev->key->xkbInfo;
    xkb = xkbi->desc;

    if ((xkb->min_key_code != req->minKeyCode)||
        (xkb->max_key_code != req->maxKeyCode)) {
	if (client->vMajor!=1) { /* pre 1.0 versions of Xlib have a bug */
	    req->minKeyCode= xkb->min_key_code;
	    req->maxKeyCode= xkb->max_key_code;
	}
	else {
	    if (!XkbIsLegalKeycode(req->minKeyCode)) {
		client->errorValue = _XkbErrCode3(2, req->minKeyCode, req->maxKeyCode);
		return BadValue;
	    }
	    if (req->minKeyCode > req->maxKeyCode) {
		client->errorValue = _XkbErrCode3(3, req->minKeyCode, req->maxKeyCode);
		return BadMatch;
	    }
	}
    }

    if ((req->present & XkbKeyTypesMask) &&
	(!CheckKeyTypes(client,xkb,req,(xkbKeyTypeWireDesc **)&values,
						&nTypes,mapWidths))) {
	client->errorValue = nTypes;
	return BadValue;
    }
    if ((req->present & XkbKeySymsMask) &&
	(!CheckKeySyms(client,xkb,req,nTypes,mapWidths,symsPerKey,
					(xkbSymMapWireDesc **)&values,&error))) {
	client->errorValue = error;
	return BadValue;
    }

    if ((req->present & XkbKeyActionsMask) &&
	(!CheckKeyActions(xkb,req,nTypes,mapWidths,symsPerKey,
						(CARD8 **)&values,&nActions))) {
	client->errorValue = nActions;
	return BadValue;
    }

    if ((req->present & XkbKeyBehaviorsMask) &&
	(!CheckKeyBehaviors(xkb,req,(xkbBehaviorWireDesc**)&values,&error))) {
	client->errorValue = error;
	return BadValue;
    }

    if ((req->present & XkbVirtualModsMask) &&
	(!CheckVirtualMods(xkb,req,(CARD8 **)&values,&error))) {
	client->errorValue= error;
	return BadValue;
    }
    if ((req->present&XkbExplicitComponentsMask) &&
	(!CheckKeyExplicit(xkb,req,(CARD8 **)&values,&error))) {
	client->errorValue= error;
	return BadValue;
    }
    if ((req->present&XkbModifierMapMask) &&
	(!CheckModifierMap(xkb,req,(CARD8 **)&values,&error))) {
	client->errorValue= error;
	return BadValue;
    }
    if ((req->present&XkbVirtualModMapMask) &&
	(!CheckVirtualModMap(xkb,req,(xkbVModMapWireDesc **)&values,&error))) {
	client->errorValue= error;
	return BadValue;
    }

    if (((values-((char *)req))/4)!= req->length) {
	ErrorF("[xkb] Internal error! Bad length in XkbSetMap (after check)\n");
	client->errorValue = values-((char *)&req[1]);
	return BadLength;
    }

    return Success;
}