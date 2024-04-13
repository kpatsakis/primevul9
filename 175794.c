is_device_installed(gx_device *dev, const char *name)
{
    while (dev) {
        if (!strcmp(dev->dname, name)) {
            return true;
        }
        dev = dev->child;
    }
    return false;
}