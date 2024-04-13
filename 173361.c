static void _print_parity(struct bnx2x *bp, u32 reg)
{
	pr_cont(" [0x%08x] ", REG_RD(bp, reg));
}