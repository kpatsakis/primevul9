static inline struct intel_iommu *dev_to_intel_iommu(struct device *dev)
{
	struct iommu_device *iommu_dev = dev_to_iommu_device(dev);

	return container_of(iommu_dev, struct intel_iommu, iommu);
}