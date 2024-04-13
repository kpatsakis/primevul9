static struct dmar_domain *find_domain(struct device *dev)
{
	struct device_domain_info *info;

	/* No lock here, assumes no domain exit in normal case */
	info = dev->archdata.iommu;
	if (likely(info))
		return info->domain;
	return NULL;
}