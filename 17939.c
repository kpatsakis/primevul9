ProcXkbGetCompatMap(ClientPtr client)
{
    xkbGetCompatMapReply 	rep;
    DeviceIntPtr 		dev;
    XkbDescPtr			xkb;
    XkbCompatMapPtr		compat;

    REQUEST(xkbGetCompatMapReq);
    REQUEST_SIZE_MATCH(xkbGetCompatMapReq);

    if (!(client->xkbClientFlags&_XkbClientInitialized))
	return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixGetAttrAccess);

    xkb = dev->key->xkbInfo->desc;
    compat= xkb->compat;

    rep.type = X_Reply;
    rep.deviceID = dev->id;
    rep.sequenceNumber = client->sequence;
    rep.length = 0;
    rep.firstSI = stuff->firstSI;
    rep.nSI = stuff->nSI;
    if (stuff->getAllSI) {
	rep.firstSI = 0;
	rep.nSI = compat->num_si;
    }
    else if ((((unsigned)stuff->nSI)>0)&&
		((unsigned)(stuff->firstSI+stuff->nSI-1)>=compat->num_si)) {
	client->errorValue = _XkbErrCode2(0x05,compat->num_si);
	return BadValue;
    }
    rep.nTotalSI = compat->num_si;
    rep.groups= stuff->groups;
    XkbComputeGetCompatMapReplySize(compat,&rep);
    return XkbSendCompatMap(client,compat,&rep);
}