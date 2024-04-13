ProcXkbGetNames(ClientPtr client)
{
    DeviceIntPtr	dev;
    XkbDescPtr		xkb;
    xkbGetNamesReply 	rep;

    REQUEST(xkbGetNamesReq);
    REQUEST_SIZE_MATCH(xkbGetNamesReq);

    if (!(client->xkbClientFlags&_XkbClientInitialized))
	return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixGetAttrAccess);
    CHK_MASK_LEGAL(0x01,stuff->which,XkbAllNamesMask);

    xkb = dev->key->xkbInfo->desc;
    rep.type= X_Reply;
    rep.sequenceNumber= client->sequence;
    rep.length = 0;
    rep.deviceID = dev->id;
    rep.which = stuff->which;
    rep.nTypes = xkb->map->num_types;
    rep.firstKey = xkb->min_key_code;
    rep.nKeys = XkbNumKeys(xkb);
    if (xkb->names!=NULL) {
	rep.nKeyAliases= xkb->names->num_key_aliases;
	rep.nRadioGroups = xkb->names->num_rg;
    }
    else {
	rep.nKeyAliases= rep.nRadioGroups= 0;
    }
    XkbComputeGetNamesReplySize(xkb,&rep);
    return XkbSendNames(client,xkb,&rep);
}