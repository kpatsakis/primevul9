ProcXkbUseExtension(ClientPtr client)
{
    REQUEST(xkbUseExtensionReq);
    xkbUseExtensionReply	rep;
    register int n;
    int	supported;

    REQUEST_SIZE_MATCH(xkbUseExtensionReq);
    if (stuff->wantedMajor != XkbMajorVersion) {
	/* pre-release version 0.65 is compatible with 1.00 */
	supported= ((XkbMajorVersion==1)&&
		    (stuff->wantedMajor==0)&&(stuff->wantedMinor==65));
    }
    else supported = 1;

    if ((supported) && (!(client->xkbClientFlags&_XkbClientInitialized))) {
	client->xkbClientFlags= _XkbClientInitialized;
	client->vMajor= stuff->wantedMajor;
	client->vMinor= stuff->wantedMinor;
    }
    else if (xkbDebugFlags&0x1) {
	ErrorF("[xkb] Rejecting client %d (0x%lx) (wants %d.%02d, have %d.%02d)\n",
					client->index,
					(long)client->clientAsMask,
					stuff->wantedMajor,stuff->wantedMinor,
					XkbMajorVersion,XkbMinorVersion);
    }
    rep.type = X_Reply;
    rep.supported = supported;
    rep.length = 0;
    rep.sequenceNumber = client->sequence;
    rep.serverMajor = XkbMajorVersion;
    rep.serverMinor = XkbMinorVersion;
    if ( client->swapped ) {
	swaps(&rep.sequenceNumber, n);
	swaps(&rep.serverMajor, n);
	swaps(&rep.serverMinor, n);
    }
    WriteToClient(client,SIZEOF(xkbUseExtensionReply), (char *)&rep);
    return client->noClientException;
}