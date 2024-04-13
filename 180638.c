static int identity_mapping(struct device *dev)
{
	struct device_domain_info *info;

	if (likely(!iommu_identity_mapping))
		return 0;

	info = dev->archdata.iommu;
	if (info && info != DUMMY_DEVICE_DOMAIN_INFO)
		return (info->domain == si_domain);

	return 0;
}