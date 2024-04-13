static void mce_device_release(struct device *dev)
{
	kfree(dev);
}