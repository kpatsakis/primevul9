static void intel_iommu_remove_device(struct device *dev)
{
	struct intel_iommu *iommu;
	u8 bus, devfn;

	iommu = device_to_iommu(dev, &bus, &devfn);
	if (!iommu)
		return;

	iommu_group_remove_device(dev);

	iommu_device_unlink(&iommu->iommu, dev);
}