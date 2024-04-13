device_wants_optimization(gx_device *dev)
{
    return (dev_proc(dev, fillpage) == gx_default_fillpage);
}