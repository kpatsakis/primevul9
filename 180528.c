iommu_support_dev_iotlb (struct dmar_domain *domain, struct intel_iommu *iommu,
			 u8 bus, u8 devfn)
{
	struct device_domain_info *info;

	assert_spin_locked(&device_domain_lock);

	if (!iommu->qi)
		return NULL;

	list_for_each_entry(info, &domain->devices, link)
		if (info->iommu == iommu && info->bus == bus &&
		    info->devfn == devfn) {
			if (info->ats_supported && info->dev)
				return info;
			break;
		}

	return NULL;
}