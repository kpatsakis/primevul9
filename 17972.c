ProcXkbSetNamedIndicator(ClientPtr client)
{
    int                         rc;
    DeviceIntPtr                dev;
    int                         led = 0;
    XkbIndicatorMapPtr          map;

    REQUEST(xkbSetNamedIndicatorReq);
    REQUEST_SIZE_MATCH(xkbSetNamedIndicatorReq);

    if (!(client->xkbClientFlags&_XkbClientInitialized))
	return BadAccess;

    CHK_LED_DEVICE(dev, stuff->deviceSpec, client, DixSetAttrAccess);
    CHK_ATOM_ONLY(stuff->indicator);
    CHK_MASK_LEGAL(0x10,stuff->whichGroups,XkbIM_UseAnyGroup);
    CHK_MASK_LEGAL(0x11,stuff->whichMods,XkbIM_UseAnyMods);

    /* Dry-run for checks */
    rc = _XkbCreateIndicatorMap(dev, stuff->indicator,
                                stuff->ledClass, stuff->ledID,
                                &map, &led, TRUE);
    if (rc != Success || !map) /* couldn't be created or didn't exist */
        return rc;

    if (stuff->deviceSpec == XkbUseCoreKbd ||
        stuff->deviceSpec == XkbUseCorePtr)
    {
        DeviceIntPtr other;
        for (other = inputInfo.devices; other; other = other->next)
        {
            if ((other != dev) && !other->isMaster && (other->u.master == dev) &&
                (XaceHook(XACE_DEVICE_ACCESS, client, other, DixSetAttrAccess) == Success))
            {
                rc = _XkbCreateIndicatorMap(other, stuff->indicator,
                                            stuff->ledClass, stuff->ledID,
                                            &map, &led, TRUE);
                if (rc != Success || !map)
                    return rc;
            }
        }
    }

    /* All checks passed, let's do it */
    rc = _XkbSetNamedIndicator(client, dev, stuff);
    if (rc != Success)
        return rc;

    if (stuff->deviceSpec == XkbUseCoreKbd ||
        stuff->deviceSpec == XkbUseCorePtr)
    {
        DeviceIntPtr other;
        for (other = inputInfo.devices; other; other = other->next)
        {
            if ((other != dev) && !other->isMaster && (other->u.master == dev) &&
                (XaceHook(XACE_DEVICE_ACCESS, client, other, DixSetAttrAccess) == Success))
            {
                _XkbSetNamedIndicator(client, other, stuff);
            }
        }
    }

    return client->noClientException;
}