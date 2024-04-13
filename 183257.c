static void intel_iommu_detach_device(struct iommu_domain *domain,
				      struct device *dev)
{
	dmar_remove_one_dev_info(dev);
}