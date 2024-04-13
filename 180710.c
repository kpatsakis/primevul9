static int iommu_dummy(struct device *dev)
{
	return dev->archdata.iommu == DUMMY_DEVICE_DOMAIN_INFO;
}