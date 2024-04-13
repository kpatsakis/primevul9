static void bnx2x_clear_func_ilt(struct bnx2x *bp, u32 func)
{
	u32 i, base = FUNC_ILT_BASE(func);
	for (i = base; i < base + ILT_PER_FUNC; i++)
		bnx2x_ilt_wr(bp, i, 0);
}