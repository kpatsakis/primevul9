ProcXkbListComponents(ClientPtr client)
{
    DeviceIntPtr 		dev;
    xkbListComponentsReply 	rep;
    unsigned			len;
    int				status;
    unsigned char *		str;
    XkbSrvListInfoRec		list;

    REQUEST(xkbListComponentsReq);
    REQUEST_AT_LEAST_SIZE(xkbListComponentsReq);

    if (!(client->xkbClientFlags&_XkbClientInitialized))
	return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixGetAttrAccess);

    status= Success;
    str= (unsigned char *)&stuff[1];
    bzero(&list,sizeof(XkbSrvListInfoRec));
    list.maxRtrn= stuff->maxNames;
    list.pattern[_XkbListKeymaps]= GetComponentSpec(&str,False,&status);
    list.pattern[_XkbListKeycodes]= GetComponentSpec(&str,False,&status);
    list.pattern[_XkbListTypes]= GetComponentSpec(&str,False,&status);
    list.pattern[_XkbListCompat]= GetComponentSpec(&str,False,&status);
    list.pattern[_XkbListSymbols]= GetComponentSpec(&str,False,&status);
    list.pattern[_XkbListGeometry]= GetComponentSpec(&str,False,&status);
    if (status!=Success)
	return status;
    len= str-((unsigned char *)stuff);
    if ((XkbPaddedSize(len)/4)!=stuff->length)
	return BadLength;
    if ((status=XkbDDXList(dev,&list,client))!=Success) {
	if (list.pool) {
	    _XkbFree(list.pool);
	    list.pool= NULL;
	}
	return status;
    }
    bzero(&rep,sizeof(xkbListComponentsReply));
    rep.type= X_Reply;
    rep.deviceID = dev->id;
    rep.sequenceNumber = client->sequence;
    rep.length = XkbPaddedSize(list.nPool)/4;
    rep.nKeymaps = list.nFound[_XkbListKeymaps];
    rep.nKeycodes = list.nFound[_XkbListKeycodes];
    rep.nTypes = list.nFound[_XkbListTypes];
    rep.nCompatMaps = list.nFound[_XkbListCompat];
    rep.nSymbols = list.nFound[_XkbListSymbols];
    rep.nGeometries = list.nFound[_XkbListGeometry];
    rep.extra=	0;
    if (list.nTotal>list.maxRtrn)
	rep.extra = (list.nTotal-list.maxRtrn);
    if (client->swapped) {
	register int n;
	swaps(&rep.sequenceNumber,n);
	swapl(&rep.length,n);
	swaps(&rep.nKeymaps,n);
	swaps(&rep.nKeycodes,n);
	swaps(&rep.nTypes,n);
	swaps(&rep.nCompatMaps,n);
	swaps(&rep.nSymbols,n);
	swaps(&rep.nGeometries,n);
	swaps(&rep.extra,n);
    }
    WriteToClient(client,SIZEOF(xkbListComponentsReply),(char *)&rep);
    if (list.nPool && list.pool) {
	WriteToClient(client,XkbPaddedSize(list.nPool), (char *)list.pool);
	_XkbFree(list.pool);
	list.pool= NULL;
    }
    return client->noClientException;
}