ProcXkbGetIndicatorMap(ClientPtr client)
{
xkbGetIndicatorMapReply rep;
DeviceIntPtr		dev;
XkbDescPtr		xkb;
XkbIndicatorPtr		leds;

    REQUEST(xkbGetIndicatorMapReq);
    REQUEST_SIZE_MATCH(xkbGetIndicatorMapReq);

    if (!(client->xkbClientFlags&_XkbClientInitialized))
	return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixGetAttrAccess);

    xkb= dev->key->xkbInfo->desc;
    leds= xkb->indicators;

    rep.type = X_Reply;
    rep.sequenceNumber = client->sequence;
    rep.length = 0;
    rep.deviceID = dev->id;
    rep.which = stuff->which;
    XkbComputeGetIndicatorMapReplySize(leds,&rep);
    return XkbSendIndicatorMap(client,leds,&rep);
}