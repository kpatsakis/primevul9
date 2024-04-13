static int device_context_mapped(struct intel_iommu *iommu, u8 bus, u8 devfn)
{
	struct context_entry *context;
	int ret = 0;
	unsigned long flags;

	spin_lock_irqsave(&iommu->lock, flags);
	context = iommu_context_addr(iommu, bus, devfn, 0);
	if (context)
		ret = context_present(context);
	spin_unlock_irqrestore(&iommu->lock, flags);
	return ret;
}