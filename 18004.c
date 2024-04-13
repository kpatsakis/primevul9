ProcXkbSetGeometry(ClientPtr client)
{
    DeviceIntPtr        dev;
    int                 rc;

    REQUEST(xkbSetGeometryReq);
    REQUEST_AT_LEAST_SIZE(xkbSetGeometryReq);

    if (!(client->xkbClientFlags&_XkbClientInitialized))
	return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, DixManageAccess);
    CHK_ATOM_OR_NONE(stuff->name);

    rc = _XkbSetGeometry(client, dev, stuff);
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
                    _XkbSetGeometry(client, other, stuff);
            }
        }
    }

    return Success;
}