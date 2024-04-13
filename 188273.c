static u32 _temac_ior_be(struct temac_local *lp, int offset)
{
	return ioread32be(lp->regs + offset);
}