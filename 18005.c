ProcXkbSetMap(ClientPtr client)
{
    DeviceIntPtr	dev;
    char *		tmp;
    int                 rc;

    REQUEST(xkbSetMapReq);
    REQUEST_AT_LEAST_SIZE(xkbSetMapReq);

    if (!(client->xkbClientFlags&_XkbClientInitialized))
	return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixManageAccess);
    CHK_MASK_LEGAL(0x01,stuff->present,XkbAllMapComponentsMask);

    tmp = (char *)&stuff[1];

    /* Check if we can to the SetMap on the requested device. If this
       succeeds, do the same thing for all extension devices (if needed).
       If any of them fails, fail.  */
    rc = _XkbSetMapChecks(client, dev, stuff, tmp);

    if (rc != Success)
        return rc;

    if (stuff->deviceSpec == XkbUseCoreKbd)
    {
        DeviceIntPtr other;
        for (other = inputInfo.devices; other; other = other->next)
        {
            if ((other != dev) && other->key && !other->isMaster && (other->u.master == dev))
            {
                rc = XaceHook(XACE_DEVICE_ACCESS, client, other, DixManageAccess);
                if (rc == Success)
                {
                    rc = _XkbSetMapChecks(client, other, stuff, tmp);
                    if (rc != Success)
                        return rc;
                }
            }
        }
    }

    /* We know now that we will succed with the SetMap. In theory anyway. */
    rc = _XkbSetMap(client, dev, stuff, tmp);
    if (rc != Success)
        return rc;

    if (stuff->deviceSpec == XkbUseCoreKbd)
    {
        DeviceIntPtr other;
        for (other = inputInfo.devices; other; other = other->next)
        {
            if ((other != dev) && other->key && !other->isMaster && (other->u.master == dev))
            {
                rc = XaceHook(XACE_DEVICE_ACCESS, client, other, DixManageAccess);
                if (rc == Success)
                    _XkbSetMap(client, other, stuff, tmp);
                /* ignore rc. if the SetMap failed although the check above
                   reported true there isn't much we can do. we still need to
                   set all other devices, hoping that at least they stay in
                   sync. */
            }
        }
    }

    return client->noClientException;
}