static int __init dev_prepare_static_identity_mapping(struct device *dev, int hw)
{
	int ret;

	if (!iommu_should_identity_map(dev, 1))
		return 0;

	ret = domain_add_dev_info(si_domain, dev);
	if (!ret)
		pr_info("%s identity mapping for device %s\n",
			hw ? "Hardware" : "Software", dev_name(dev));
	else if (ret == -ENODEV)
		/* device not associated with an iommu */
		ret = 0;

	return ret;
}