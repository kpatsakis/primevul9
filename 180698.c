static void domain_context_clear_one(struct intel_iommu *iommu, u8 bus, u8 devfn)
{
	unsigned long flags;
	struct context_entry *context;
	u16 did_old;

	if (!iommu)
		return;

	spin_lock_irqsave(&iommu->lock, flags);
	context = iommu_context_addr(iommu, bus, devfn, 0);
	if (!context) {
		spin_unlock_irqrestore(&iommu->lock, flags);
		return;
	}
	did_old = context_domain_id(context);
	context_clear_entry(context);
	__iommu_flush_cache(iommu, context, sizeof(*context));
	spin_unlock_irqrestore(&iommu->lock, flags);
	iommu->flush.flush_context(iommu,
				   did_old,
				   (((u16)bus) << 8) | devfn,
				   DMA_CCMD_MASK_NOBIT,
				   DMA_CCMD_DEVICE_INVL);
	iommu->flush.flush_iotlb(iommu,
				 did_old,
				 0,
				 0,
				 DMA_TLB_DSI_FLUSH);
}