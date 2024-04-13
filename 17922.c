ProcXkbSetDeviceInfo(ClientPtr client)
{
    unsigned int        change;
    DeviceIntPtr        dev;
    int                 rc;

    REQUEST(xkbSetDeviceInfoReq);
    REQUEST_AT_LEAST_SIZE(xkbSetDeviceInfoReq);

    if (!(client->xkbClientFlags&_XkbClientInitialized))
	return BadAccess;

    change = stuff->change;

    CHK_ANY_DEVICE(dev, stuff->deviceSpec, client, DixManageAccess);
    CHK_MASK_LEGAL(0x01,change,XkbXI_AllFeaturesMask);

    rc = _XkbSetDeviceInfoCheck(client, dev, stuff);

    if (rc != Success)
        return rc;

    if (stuff->deviceSpec == XkbUseCoreKbd || stuff->deviceSpec == XkbUseCorePtr)
    {
        DeviceIntPtr other;
        for (other = inputInfo.devices; other; other = other->next)
        {
            if (((other != dev) && !other->isMaster && (other->u.master == dev)) &&
                ((stuff->deviceSpec == XkbUseCoreKbd && other->key) ||
                (stuff->deviceSpec == XkbUseCorePtr && other->button)))
            {
                rc = XaceHook(XACE_DEVICE_ACCESS, client, other, DixManageAccess);
                if (rc == Success)
                {
                    rc = _XkbSetDeviceInfoCheck(client, other, stuff);
                    if (rc != Success)
                        return rc;
                }
            }
        }
    }

    /* checks done, apply */
    rc = _XkbSetDeviceInfo(client, dev, stuff);
    if (rc != Success)
        return rc;

    if (stuff->deviceSpec == XkbUseCoreKbd || stuff->deviceSpec == XkbUseCorePtr)
    {
        DeviceIntPtr other;
        for (other = inputInfo.devices; other; other = other->next)
        {
            if (((other != dev) && !other->isMaster && (other->u.master == dev)) &&
                ((stuff->deviceSpec == XkbUseCoreKbd && other->key) ||
                (stuff->deviceSpec == XkbUseCorePtr && other->button)))
            {
                rc = XaceHook(XACE_DEVICE_ACCESS, client, other, DixManageAccess);
                if (rc == Success)
                {
                    rc = _XkbSetDeviceInfo(client, other, stuff);
                    if (rc != Success)
                        return rc;
                }
            }
        }
    }

    return client->noClientException;
}