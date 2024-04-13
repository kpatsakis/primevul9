XkbClientGone(pointer data,XID id)
{
    DevicePtr	pXDev = (DevicePtr)data;

    if (!XkbRemoveResourceClient(pXDev,id)) {
	ErrorF("[xkb] Internal Error! bad RemoveResourceClient in XkbClientGone\n");
    }
    return 1;
}