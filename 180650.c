static ssize_t intel_iommu_show_version(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct intel_iommu *iommu = dev_to_intel_iommu(dev);
	u32 ver = readl(iommu->reg + DMAR_VER_REG);
	return sprintf(buf, "%d:%d\n",
		       DMAR_VER_MAJOR(ver), DMAR_VER_MINOR(ver));
}