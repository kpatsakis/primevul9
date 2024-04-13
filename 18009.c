ProcXkbGetIndicatorState(ClientPtr client)
{
    xkbGetIndicatorStateReply 	rep;
    XkbSrvLedInfoPtr		sli;
    DeviceIntPtr 		dev;
    register int 		i;

    REQUEST(xkbGetIndicatorStateReq);
    REQUEST_SIZE_MATCH(xkbGetIndicatorStateReq);

    if (!(client->xkbClientFlags&_XkbClientInitialized))
	return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixReadAccess);

    sli= XkbFindSrvLedInfo(dev,XkbDfltXIClass,XkbDfltXIId,
						XkbXI_IndicatorStateMask);
    if (!sli)
	return BadAlloc;

    rep.type = X_Reply;
    rep.sequenceNumber = client->sequence;
    rep.length = 0;
    rep.deviceID = dev->id;
    rep.state = sli->effectiveState;

    if (client->swapped) {
	swaps(&rep.sequenceNumber,i);
	swapl(&rep.state,i);
    }
    WriteToClient(client, SIZEOF(xkbGetIndicatorStateReply), (char *)&rep);
    return client->noClientException;
}