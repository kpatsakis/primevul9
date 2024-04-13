static void temac_dma_out32_be(struct temac_local *lp, int reg, u32 value)
{
	iowrite32be(value, lp->sdma_regs + (reg << 2));
}