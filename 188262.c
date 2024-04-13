static u32 temac_dma_dcr_in(struct temac_local *lp, int reg)
{
	return dcr_read(lp->sdma_dcrs, reg);
}