ProcXkbSetDebuggingFlags(ClientPtr client)
{
CARD32 				newFlags,newCtrls,extraLength;
xkbSetDebuggingFlagsReply 	rep;
int rc;

    REQUEST(xkbSetDebuggingFlagsReq);
    REQUEST_AT_LEAST_SIZE(xkbSetDebuggingFlagsReq);

    rc = XaceHook(XACE_SERVER_ACCESS, client, DixDebugAccess);
    if (rc != Success)
	return rc;

    newFlags=  xkbDebugFlags&(~stuff->affectFlags);
    newFlags|= (stuff->flags&stuff->affectFlags);
    newCtrls=  xkbDebugCtrls&(~stuff->affectCtrls);
    newCtrls|= (stuff->ctrls&stuff->affectCtrls);
    if (xkbDebugFlags || newFlags || stuff->msgLength) {
	ErrorF("[xkb] XkbDebug: Setting debug flags to 0x%lx\n",(long)newFlags);
	if (newCtrls!=xkbDebugCtrls)
	    ErrorF("[xkb] XkbDebug: Setting debug controls to 0x%lx\n",(long)newCtrls);
    }
    extraLength= (stuff->length<<2)-sz_xkbSetDebuggingFlagsReq;
    if (stuff->msgLength>0) {
	char *msg;
	if (extraLength<XkbPaddedSize(stuff->msgLength)) {
	    ErrorF("[xkb] XkbDebug: msgLength= %d, length= %ld (should be %d)\n",
			stuff->msgLength,(long)extraLength,
			XkbPaddedSize(stuff->msgLength));
	    return BadLength;
	}
	msg= (char *)&stuff[1];
	if (msg[stuff->msgLength-1]!='\0') {
	    ErrorF("[xkb] XkbDebug: message not null-terminated\n");
	    return BadValue;
	}
	ErrorF("[xkb] XkbDebug: %s\n",msg);
    }
    xkbDebugFlags = newFlags;
    xkbDebugCtrls = newCtrls;

    XkbDisableLockActions= (xkbDebugCtrls&XkbDF_DisableLocks);

    rep.type= X_Reply;
    rep.length = 0;
    rep.sequenceNumber = client->sequence;
    rep.currentFlags = newFlags;
    rep.currentCtrls = newCtrls;
    rep.supportedFlags = ~0;
    rep.supportedCtrls = ~0;
    if ( client->swapped ) {
	register int n;
	swaps(&rep.sequenceNumber, n);
	swapl(&rep.currentFlags, n);
	swapl(&rep.currentCtrls, n);
	swapl(&rep.supportedFlags, n);
	swapl(&rep.supportedCtrls, n);
    }
    WriteToClient(client,SIZEOF(xkbSetDebuggingFlagsReply), (char *)&rep);
    return client->noClientException;
}