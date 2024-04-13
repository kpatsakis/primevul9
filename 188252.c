static void _temac_iow_le(struct temac_local *lp, int offset, u32 value)
{
	return iowrite32(value, lp->regs + offset);
}