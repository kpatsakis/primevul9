struct intel_iommu *intel_svm_device_to_iommu(struct device *dev)
{
	struct intel_iommu *iommu;
	u8 bus, devfn;

	if (iommu_dummy(dev)) {
		dev_warn(dev,
			 "No IOMMU translation for device; cannot enable SVM\n");
		return NULL;
	}

	iommu = device_to_iommu(dev, &bus, &devfn);
	if ((!iommu)) {
		dev_err(dev, "No IOMMU for device; cannot enable SVM\n");
		return NULL;
	}

	return iommu;
}