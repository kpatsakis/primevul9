static int intel_iommu_add_device(struct device *dev)
{
	struct intel_iommu *iommu;
	struct iommu_group *group;
	u8 bus, devfn;

	iommu = device_to_iommu(dev, &bus, &devfn);
	if (!iommu)
		return -ENODEV;

	iommu_device_link(&iommu->iommu, dev);

	group = iommu_group_get_for_dev(dev);

	if (IS_ERR(group))
		return PTR_ERR(group);

	iommu_group_put(group);
	return 0;
}