static u32 _temac_ior_le(struct temac_local *lp, int offset)
{
	return ioread32(lp->regs + offset);
}