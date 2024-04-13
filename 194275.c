int amba_device_add(struct amba_device *dev, struct resource *parent)
{
	u32 size;
	void __iomem *tmp;
	int i, ret;

	WARN_ON(dev->irq[0] == (unsigned int)-1);
	WARN_ON(dev->irq[1] == (unsigned int)-1);

	ret = request_resource(parent, &dev->res);
	if (ret)
		goto err_out;

	/* Hard-coded primecell ID instead of plug-n-play */
	if (dev->periphid != 0)
		goto skip_probe;

	/*
	 * Dynamically calculate the size of the resource
	 * and use this for iomap
	 */
	size = resource_size(&dev->res);
	tmp = ioremap(dev->res.start, size);
	if (!tmp) {
		ret = -ENOMEM;
		goto err_release;
	}

	ret = amba_get_enable_pclk(dev);
	if (ret == 0) {
		u32 pid, cid;

		/*
		 * Read pid and cid based on size of resource
		 * they are located at end of region
		 */
		for (pid = 0, i = 0; i < 4; i++)
			pid |= (readl(tmp + size - 0x20 + 4 * i) & 255) <<
				(i * 8);
		for (cid = 0, i = 0; i < 4; i++)
			cid |= (readl(tmp + size - 0x10 + 4 * i) & 255) <<
				(i * 8);

		amba_put_disable_pclk(dev);

		if (cid == AMBA_CID || cid == CORESIGHT_CID)
			dev->periphid = pid;

		if (!dev->periphid)
			ret = -ENODEV;
	}

	iounmap(tmp);

	if (ret)
		goto err_release;

 skip_probe:
	ret = device_add(&dev->dev);
	if (ret)
		goto err_release;

	if (dev->irq[0])
		ret = device_create_file(&dev->dev, &dev_attr_irq0);
	if (ret == 0 && dev->irq[1])
		ret = device_create_file(&dev->dev, &dev_attr_irq1);
	if (ret == 0)
		return ret;

	device_unregister(&dev->dev);

 err_release:
	release_resource(&dev->res);
 err_out:
	return ret;
}