static void bnx2x_clean_pglue_errors(struct bnx2x *bp)
{
	if (!CHIP_IS_E1x(bp))
		REG_WR(bp, PGLUE_B_REG_WAS_ERROR_PF_7_0_CLR,
		       1 << BP_ABS_FUNC(bp));
}