static int domain_add_dev_info(struct dmar_domain *domain, struct device *dev)
{
	struct dmar_domain *ndomain;
	struct intel_iommu *iommu;
	u8 bus, devfn;

	iommu = device_to_iommu(dev, &bus, &devfn);
	if (!iommu)
		return -ENODEV;

	ndomain = dmar_insert_one_dev_info(iommu, bus, devfn, dev, domain);
	if (ndomain != domain)
		return -EBUSY;

	return 0;
}