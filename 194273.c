static int amba_probe(struct device *dev)
{
	struct amba_device *pcdev = to_amba_device(dev);
	struct amba_driver *pcdrv = to_amba_driver(dev->driver);
	const struct amba_id *id = amba_lookup(pcdrv->id_table, pcdev);
	int ret;

	do {
		ret = dev_pm_domain_attach(dev, true);
		if (ret == -EPROBE_DEFER)
			break;

		ret = amba_get_enable_pclk(pcdev);
		if (ret) {
			dev_pm_domain_detach(dev, true);
			break;
		}

		pm_runtime_get_noresume(dev);
		pm_runtime_set_active(dev);
		pm_runtime_enable(dev);

		ret = pcdrv->probe(pcdev, id);
		if (ret == 0)
			break;

		pm_runtime_disable(dev);
		pm_runtime_set_suspended(dev);
		pm_runtime_put_noidle(dev);

		amba_put_disable_pclk(pcdev);
		dev_pm_domain_detach(dev, true);
	} while (0);

	return ret;
}