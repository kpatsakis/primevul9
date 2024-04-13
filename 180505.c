static int iommu_no_mapping(struct device *dev)
{
	int found;

	if (iommu_dummy(dev))
		return 1;

	if (!iommu_identity_mapping)
		return 0;

	found = identity_mapping(dev);
	if (found) {
		if (iommu_should_identity_map(dev, 0))
			return 1;
		else {
			/*
			 * 32 bit DMA is removed from si_domain and fall back
			 * to non-identity mapping.
			 */
			dmar_remove_one_dev_info(si_domain, dev);
			pr_info("32bit %s uses non-identity mapping\n",
				dev_name(dev));
			return 0;
		}
	} else {
		/*
		 * In case of a detached 64 bit DMA device from vm, the device
		 * is put into si_domain for identity mapping.
		 */
		if (iommu_should_identity_map(dev, 0)) {
			int ret;
			ret = domain_add_dev_info(si_domain, dev);
			if (!ret) {
				pr_info("64bit %s uses identity mapping\n",
					dev_name(dev));
				return 1;
			}
		}
	}

	return 0;
}