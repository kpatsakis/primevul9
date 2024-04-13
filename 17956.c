ProcXkbSetIndicatorMap(ClientPtr client)
{
    int                 i, bit;
    int                 nIndicators;
    DeviceIntPtr        dev;
    xkbIndicatorMapWireDesc     *from;
    int                 rc;

    REQUEST(xkbSetIndicatorMapReq);
    REQUEST_AT_LEAST_SIZE(xkbSetIndicatorMapReq);

    if (!(client->xkbClientFlags&_XkbClientInitialized))
	return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixSetAttrAccess);

    if (stuff->which==0)
	return client->noClientException;

    for (nIndicators=i=0,bit=1;i<XkbNumIndicators;i++,bit<<=1) {
	if (stuff->which&bit)
	    nIndicators++;
    }
    if (stuff->length!=((SIZEOF(xkbSetIndicatorMapReq)+
			(nIndicators*SIZEOF(xkbIndicatorMapWireDesc)))/4)) {
	return BadLength;
    }

    from = (xkbIndicatorMapWireDesc *)&stuff[1];
    for (i=0,bit=1;i<XkbNumIndicators;i++,bit<<=1) {
	if (stuff->which&bit) {
	    if (client->swapped) {
		int n;
		swaps(&from->virtualMods,n);
		swapl(&from->ctrls,n);
	    }
	    CHK_MASK_LEGAL(i,from->whichGroups,XkbIM_UseAnyGroup);
	    CHK_MASK_LEGAL(i,from->whichMods,XkbIM_UseAnyMods);
	    from++;
	}
    }

    from = (xkbIndicatorMapWireDesc *)&stuff[1];
    rc = _XkbSetIndicatorMap(client, dev, stuff->which, from);
    if (rc != Success)
        return rc;

    if (stuff->deviceSpec == XkbUseCoreKbd)
    {
        DeviceIntPtr other;
        for (other = inputInfo.devices; other; other = other->next)
        {
            if ((other != dev) && other->key && !other->isMaster && (other->u.master == dev))
            {
                rc = XaceHook(XACE_DEVICE_ACCESS, client, other, DixSetAttrAccess);
                if (rc == Success)
                    _XkbSetIndicatorMap(client, other, stuff->which, from);
            }
        }
    }

    return Success;
}