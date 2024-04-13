static void temac_dma_dcr_out(struct temac_local *lp, int reg, u32 value)
{
	dcr_write(lp->sdma_dcrs, reg, value);
}