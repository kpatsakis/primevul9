static u32 temac_dma_in32_be(struct temac_local *lp, int reg)
{
	return ioread32be(lp->sdma_regs + (reg << 2));
}