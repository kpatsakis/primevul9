static void kvmgt_host_exit(struct device *dev, void *gvt)
{
	mdev_unregister_device(dev);
}