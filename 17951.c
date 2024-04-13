ProcXkbSetCompatMap(ClientPtr client)
{
    DeviceIntPtr        dev;
    char                *data;
    int                 rc;

    REQUEST(xkbSetCompatMapReq);
    REQUEST_AT_LEAST_SIZE(xkbSetCompatMapReq);

    if (!(client->xkbClientFlags&_XkbClientInitialized))
	return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixManageAccess);

    data = (char *)&stuff[1];

    /* check first using a dry-run */
    rc = _XkbSetCompatMap(client, dev, stuff, data, TRUE);
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
                    /* dry-run */
                    rc = _XkbSetCompatMap(client, other, stuff, data, TRUE);
                    if (rc != Success)
                        return rc;
                }
            }
        }
    }

    /* Yay, the dry-runs succeed. Let's apply */
    rc = _XkbSetCompatMap(client, dev, stuff, data, TRUE);
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
                    rc = _XkbSetCompatMap(client, other, stuff, data, TRUE);
                    if (rc != Success)
                        return rc;
                }
            }
        }
    }

    return client->noClientException;
}