static u32 temac_dma_in32_le(struct temac_local *lp, int reg)
{
	return ioread32(lp->sdma_regs + (reg << 2));
}