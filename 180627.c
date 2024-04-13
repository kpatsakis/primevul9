static void __iommu_flush_context(struct intel_iommu *iommu,
				  u16 did, u16 source_id, u8 function_mask,
				  u64 type)
{
	u64 val = 0;
	unsigned long flag;

	switch (type) {
	case DMA_CCMD_GLOBAL_INVL:
		val = DMA_CCMD_GLOBAL_INVL;
		break;
	case DMA_CCMD_DOMAIN_INVL:
		val = DMA_CCMD_DOMAIN_INVL|DMA_CCMD_DID(did);
		break;
	case DMA_CCMD_DEVICE_INVL:
		val = DMA_CCMD_DEVICE_INVL|DMA_CCMD_DID(did)
			| DMA_CCMD_SID(source_id) | DMA_CCMD_FM(function_mask);
		break;
	default:
		BUG();
	}
	val |= DMA_CCMD_ICC;

	raw_spin_lock_irqsave(&iommu->register_lock, flag);
	dmar_writeq(iommu->reg + DMAR_CCMD_REG, val);

	/* Make sure hardware complete it */
	IOMMU_WAIT_OP(iommu, DMAR_CCMD_REG,
		dmar_readq, (!(val & DMA_CCMD_ICC)), val);

	raw_spin_unlock_irqrestore(&iommu->register_lock, flag);
}