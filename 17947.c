ProcXkbGetNamedIndicator(ClientPtr client)
{
    DeviceIntPtr 		dev;
    xkbGetNamedIndicatorReply 	rep;
    register int		i = 0;
    XkbSrvLedInfoPtr		sli;
    XkbIndicatorMapPtr		map = NULL;

    REQUEST(xkbGetNamedIndicatorReq);
    REQUEST_SIZE_MATCH(xkbGetNamedIndicatorReq);

    if (!(client->xkbClientFlags&_XkbClientInitialized))
	return BadAccess;

    CHK_LED_DEVICE(dev, stuff->deviceSpec, client, DixReadAccess);
    CHK_ATOM_ONLY(stuff->indicator);

    sli= XkbFindSrvLedInfo(dev,stuff->ledClass,stuff->ledID,0);
    if (!sli)
	return BadAlloc;

    i= 0;
    map= NULL;
    if ((sli->names)&&(sli->maps)) {
        for (i=0;i<XkbNumIndicators;i++) {
            if (stuff->indicator==sli->names[i]) {
                map= &sli->maps[i];
                break;
            }
        }
    }

    rep.type= X_Reply;
    rep.length = 0;
    rep.sequenceNumber = client->sequence;
    rep.deviceID = dev->id;
    rep.indicator= stuff->indicator;
    if (map!=NULL) {
	rep.found= 		True;
	rep.on=			((sli->effectiveState&(1<<i))!=0);
	rep.realIndicator=	((sli->physIndicators&(1<<i))!=0);
	rep.ndx= 		i;
	rep.flags= 		map->flags;
	rep.whichGroups= 	map->which_groups;
	rep.groups= 		map->groups;
	rep.whichMods= 		map->which_mods;
	rep.mods= 		map->mods.mask;
	rep.realMods= 		map->mods.real_mods;
	rep.virtualMods= 	map->mods.vmods;
	rep.ctrls= 		map->ctrls;
	rep.supported= 		True;
    }
    else  {
	rep.found= 		False;
	rep.on= 		False;
	rep.realIndicator= 	False;
	rep.ndx= 		XkbNoIndicator;
	rep.flags= 		0;
	rep.whichGroups= 	0;
	rep.groups= 		0;
	rep.whichMods= 		0;
	rep.mods=		0;
	rep.realMods= 		0;
	rep.virtualMods= 	0;
	rep.ctrls= 		0;
	rep.supported= 		True;
    }
    if ( client->swapped ) {
	register int n;
	swapl(&rep.length,n);
	swaps(&rep.sequenceNumber,n);
	swapl(&rep.indicator,n);
	swaps(&rep.virtualMods,n);
	swapl(&rep.ctrls,n);
    }

    WriteToClient(client,SIZEOF(xkbGetNamedIndicatorReply), (char *)&rep);
    return client->noClientException;
}