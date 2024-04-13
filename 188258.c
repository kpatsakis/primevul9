static void temac_dma_out32_le(struct temac_local *lp, int reg, u32 value)
{
	iowrite32(value, lp->sdma_regs + (reg << 2));
}