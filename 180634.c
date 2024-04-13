static inline int iommu_prepare_rmrr_dev(struct dmar_rmrr_unit *rmrr,
					 struct device *dev)
{
	if (dev->archdata.iommu == DUMMY_DEVICE_DOMAIN_INFO)
		return 0;
	return iommu_prepare_identity_map(dev, rmrr->base_address,
					  rmrr->end_address);
}