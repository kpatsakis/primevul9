static ssize_t intel_iommu_show_ecap(struct device *dev,
				    struct device_attribute *attr,
				    char *buf)
{
	struct intel_iommu *iommu = dev_to_intel_iommu(dev);
	return sprintf(buf, "%llx\n", iommu->ecap);
}