ProcXkbGetState(ClientPtr client)
{
    REQUEST(xkbGetStateReq);
    DeviceIntPtr	dev;
    xkbGetStateReply	 rep;
    XkbStateRec		*xkb;

    REQUEST_SIZE_MATCH(xkbGetStateReq);

    if (!(client->xkbClientFlags&_XkbClientInitialized))
	return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixReadAccess);

    xkb= &dev->key->xkbInfo->state;
    bzero(&rep,sizeof(xkbGetStateReply));
    rep.type= X_Reply;
    rep.sequenceNumber= client->sequence;
    rep.length = 0;
    rep.deviceID = dev->id;
    rep.mods = dev->key->state&0xff;
    rep.baseMods = xkb->base_mods;
    rep.lockedMods = xkb->locked_mods;
    rep.latchedMods = xkb->latched_mods;
    rep.group = xkb->group;
    rep.baseGroup = xkb->base_group;
    rep.latchedGroup = xkb->latched_group;
    rep.lockedGroup = xkb->locked_group;
    rep.compatState = xkb->compat_state;
    rep.ptrBtnState = xkb->ptr_buttons;
    if (client->swapped) {
	register int n;
	swaps(&rep.sequenceNumber,n);
	swaps(&rep.ptrBtnState,n);
    }
    WriteToClient(client, SIZEOF(xkbGetStateReply), (char *)&rep);
    return client->noClientException;
}