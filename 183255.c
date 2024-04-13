int intel_iommu_enable_pasid(struct intel_iommu *iommu, struct intel_svm_dev *sdev)
{
	struct device_domain_info *info;
	struct context_entry *context;
	struct dmar_domain *domain;
	unsigned long flags;
	u64 ctx_lo;
	int ret;

	domain = get_valid_domain_for_dev(sdev->dev);
	if (!domain)
		return -EINVAL;

	spin_lock_irqsave(&device_domain_lock, flags);
	spin_lock(&iommu->lock);

	ret = -EINVAL;
	info = sdev->dev->archdata.iommu;
	if (!info || !info->pasid_supported)
		goto out;

	context = iommu_context_addr(iommu, info->bus, info->devfn, 0);
	if (WARN_ON(!context))
		goto out;

	ctx_lo = context[0].lo;

	sdev->did = domain->iommu_did[iommu->seq_id];
	sdev->sid = PCI_DEVID(info->bus, info->devfn);

	if (!(ctx_lo & CONTEXT_PASIDE)) {
		ctx_lo |= CONTEXT_PASIDE;
		context[0].lo = ctx_lo;
		wmb();
		iommu->flush.flush_context(iommu, sdev->did, sdev->sid,
					   DMA_CCMD_MASK_NOBIT,
					   DMA_CCMD_DEVICE_INVL);
	}

	/* Enable PASID support in the device, if it wasn't already */
	if (!info->pasid_enabled)
		iommu_enable_dev_iotlb(info);

	if (info->ats_enabled) {
		sdev->dev_iotlb = 1;
		sdev->qdep = info->ats_qdep;
		if (sdev->qdep >= QI_DEV_EIOTLB_MAX_INVS)
			sdev->qdep = 0;
	}
	ret = 0;

 out:
	spin_unlock(&iommu->lock);
	spin_unlock_irqrestore(&device_domain_lock, flags);

	return ret;
}