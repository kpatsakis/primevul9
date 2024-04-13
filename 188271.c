static void _temac_iow_be(struct temac_local *lp, int offset, u32 value)
{
	return iowrite32be(value, lp->regs + offset);
}