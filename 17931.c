ProcXkbGetMap(ClientPtr client)
{
    DeviceIntPtr	 dev;
    xkbGetMapReply	 rep;
    XkbDescRec		*xkb;
    int			 n,status;

    REQUEST(xkbGetMapReq);
    REQUEST_SIZE_MATCH(xkbGetMapReq);
    
    if (!(client->xkbClientFlags&_XkbClientInitialized))
	return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixGetAttrAccess);
    CHK_MASK_OVERLAP(0x01,stuff->full,stuff->partial);
    CHK_MASK_LEGAL(0x02,stuff->full,XkbAllMapComponentsMask);
    CHK_MASK_LEGAL(0x03,stuff->partial,XkbAllMapComponentsMask);

    xkb= dev->key->xkbInfo->desc;
    bzero(&rep,sizeof(xkbGetMapReply));
    rep.type= X_Reply;
    rep.sequenceNumber= client->sequence;
    rep.length = (SIZEOF(xkbGetMapReply)-SIZEOF(xGenericReply))>>2;
    rep.deviceID = dev->id;
    rep.present = stuff->partial|stuff->full;
    rep.minKeyCode = xkb->min_key_code;
    rep.maxKeyCode = xkb->max_key_code;
    if ( stuff->full&XkbKeyTypesMask ) {
	rep.firstType = 0;
	rep.nTypes = xkb->map->num_types;
    }
    else if (stuff->partial&XkbKeyTypesMask) {
	if (((unsigned)stuff->firstType+stuff->nTypes)>xkb->map->num_types) {
	    client->errorValue = _XkbErrCode4(0x04,xkb->map->num_types,
					stuff->firstType,stuff->nTypes);
	    return BadValue;
	}
	rep.firstType = stuff->firstType;
	rep.nTypes = stuff->nTypes;
    }
    else rep.nTypes = 0;
    rep.totalTypes = xkb->map->num_types;

    n= XkbNumKeys(xkb);
    if ( stuff->full&XkbKeySymsMask ) {
	rep.firstKeySym = xkb->min_key_code;
	rep.nKeySyms = n;
    }
    else if (stuff->partial&XkbKeySymsMask) {
	CHK_KEY_RANGE(0x05,stuff->firstKeySym,stuff->nKeySyms,xkb);
	rep.firstKeySym = stuff->firstKeySym;
	rep.nKeySyms = stuff->nKeySyms;
    }
    else rep.nKeySyms = 0;
    rep.totalSyms= 0;

    if ( stuff->full&XkbKeyActionsMask ) {
	rep.firstKeyAct= xkb->min_key_code;
	rep.nKeyActs= n;
    }
    else if (stuff->partial&XkbKeyActionsMask) {
	CHK_KEY_RANGE(0x07,stuff->firstKeyAct,stuff->nKeyActs,xkb);
	rep.firstKeyAct= stuff->firstKeyAct;
	rep.nKeyActs= stuff->nKeyActs;
    }
    else rep.nKeyActs= 0;
    rep.totalActs= 0;

    if ( stuff->full&XkbKeyBehaviorsMask ) {
	rep.firstKeyBehavior = xkb->min_key_code;
	rep.nKeyBehaviors = n;
    }
    else if (stuff->partial&XkbKeyBehaviorsMask) {
	CHK_KEY_RANGE(0x09,stuff->firstKeyBehavior,stuff->nKeyBehaviors,xkb);
	rep.firstKeyBehavior= stuff->firstKeyBehavior;
	rep.nKeyBehaviors= stuff->nKeyBehaviors;
    }
    else rep.nKeyBehaviors = 0;
    rep.totalKeyBehaviors= 0;

    if (stuff->full&XkbVirtualModsMask)
	rep.virtualMods= ~0;
    else if (stuff->partial&XkbVirtualModsMask)
	rep.virtualMods= stuff->virtualMods;
    
    if (stuff->full&XkbExplicitComponentsMask) {
	rep.firstKeyExplicit= xkb->min_key_code;
	rep.nKeyExplicit= n;
    }
    else if (stuff->partial&XkbExplicitComponentsMask) {
	CHK_KEY_RANGE(0x0B,stuff->firstKeyExplicit,stuff->nKeyExplicit,xkb);
	rep.firstKeyExplicit= stuff->firstKeyExplicit;
	rep.nKeyExplicit= stuff->nKeyExplicit;
    }
    else rep.nKeyExplicit = 0;
    rep.totalKeyExplicit=  0;

    if (stuff->full&XkbModifierMapMask) {
	rep.firstModMapKey= xkb->min_key_code;
	rep.nModMapKeys= n;
    }
    else if (stuff->partial&XkbModifierMapMask) {
	CHK_KEY_RANGE(0x0D,stuff->firstModMapKey,stuff->nModMapKeys,xkb);
	rep.firstModMapKey= stuff->firstModMapKey;
	rep.nModMapKeys= stuff->nModMapKeys;
    }
    else rep.nModMapKeys = 0;
    rep.totalModMapKeys= 0;

    if (stuff->full&XkbVirtualModMapMask) {
	rep.firstVModMapKey= xkb->min_key_code;
	rep.nVModMapKeys= n;
    }
    else if (stuff->partial&XkbVirtualModMapMask) {
	CHK_KEY_RANGE(0x0F,stuff->firstVModMapKey,stuff->nVModMapKeys,xkb);
	rep.firstVModMapKey= stuff->firstVModMapKey;
	rep.nVModMapKeys= stuff->nVModMapKeys;
    }
    else rep.nVModMapKeys = 0;
    rep.totalVModMapKeys= 0;

    if ((status=XkbComputeGetMapReplySize(xkb,&rep))!=Success)
	return status;
    return XkbSendMap(client,xkb,&rep);
}