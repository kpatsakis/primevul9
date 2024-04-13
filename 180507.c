static void __dmar_remove_one_dev_info(struct device_domain_info *info)
{
	struct intel_iommu *iommu;
	unsigned long flags;

	assert_spin_locked(&device_domain_lock);

	if (WARN_ON(!info))
		return;

	iommu = info->iommu;

	if (info->dev) {
		iommu_disable_dev_iotlb(info);
		domain_context_clear(iommu, info->dev);
		intel_pasid_free_table(info->dev);
	}

	unlink_domain_info(info);

	spin_lock_irqsave(&iommu->lock, flags);
	domain_detach_iommu(info->domain, iommu);
	spin_unlock_irqrestore(&iommu->lock, flags);

	free_devinfo_mem(info);
}