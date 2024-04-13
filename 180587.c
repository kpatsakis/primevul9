static void __iommu_flush_iotlb(struct intel_iommu *iommu, u16 did,
				u64 addr, unsigned int size_order, u64 type)
{
	int tlb_offset = ecap_iotlb_offset(iommu->ecap);
	u64 val = 0, val_iva = 0;
	unsigned long flag;

	switch (type) {
	case DMA_TLB_GLOBAL_FLUSH:
		/* global flush doesn't need set IVA_REG */
		val = DMA_TLB_GLOBAL_FLUSH|DMA_TLB_IVT;
		break;
	case DMA_TLB_DSI_FLUSH:
		val = DMA_TLB_DSI_FLUSH|DMA_TLB_IVT|DMA_TLB_DID(did);
		break;
	case DMA_TLB_PSI_FLUSH:
		val = DMA_TLB_PSI_FLUSH|DMA_TLB_IVT|DMA_TLB_DID(did);
		/* IH bit is passed in as part of address */
		val_iva = size_order | addr;
		break;
	default:
		BUG();
	}
	/* Note: set drain read/write */
#if 0
	/*
	 * This is probably to be super secure.. Looks like we can
	 * ignore it without any impact.
	 */
	if (cap_read_drain(iommu->cap))
		val |= DMA_TLB_READ_DRAIN;
#endif
	if (cap_write_drain(iommu->cap))
		val |= DMA_TLB_WRITE_DRAIN;

	raw_spin_lock_irqsave(&iommu->register_lock, flag);
	/* Note: Only uses first TLB reg currently */
	if (val_iva)
		dmar_writeq(iommu->reg + tlb_offset, val_iva);
	dmar_writeq(iommu->reg + tlb_offset + 8, val);

	/* Make sure hardware complete it */
	IOMMU_WAIT_OP(iommu, tlb_offset + 8,
		dmar_readq, (!(val & DMA_TLB_IVT)), val);

	raw_spin_unlock_irqrestore(&iommu->register_lock, flag);

	/* check IOTLB invalidation granularity */
	if (DMA_TLB_IAIG(val) == 0)
		pr_err("Flush IOTLB failed\n");
	if (DMA_TLB_IAIG(val) != DMA_TLB_IIRG(type))
		pr_debug("TLB flush request %Lx, actual %Lx\n",
			(unsigned long long)DMA_TLB_IIRG(type),
			(unsigned long long)DMA_TLB_IAIG(val));
}