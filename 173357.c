bool bnx2x_chk_parity_attn(struct bnx2x *bp, bool *global, bool print)
{
	struct attn_route attn = { {0} };
	int port = BP_PORT(bp);

	attn.sig[0] = REG_RD(bp,
		MISC_REG_AEU_AFTER_INVERT_1_FUNC_0 +
			     port*4);
	attn.sig[1] = REG_RD(bp,
		MISC_REG_AEU_AFTER_INVERT_2_FUNC_0 +
			     port*4);
	attn.sig[2] = REG_RD(bp,
		MISC_REG_AEU_AFTER_INVERT_3_FUNC_0 +
			     port*4);
	attn.sig[3] = REG_RD(bp,
		MISC_REG_AEU_AFTER_INVERT_4_FUNC_0 +
			     port*4);
	/* Since MCP attentions can't be disabled inside the block, we need to
	 * read AEU registers to see whether they're currently disabled
	 */
	attn.sig[3] &= ((REG_RD(bp,
				!port ? MISC_REG_AEU_ENABLE4_FUNC_0_OUT_0
				      : MISC_REG_AEU_ENABLE4_FUNC_1_OUT_0) &
			 MISC_AEU_ENABLE_MCP_PRTY_BITS) |
			~MISC_AEU_ENABLE_MCP_PRTY_BITS);

	if (!CHIP_IS_E1x(bp))
		attn.sig[4] = REG_RD(bp,
			MISC_REG_AEU_AFTER_INVERT_5_FUNC_0 +
				     port*4);

	return bnx2x_parity_attn(bp, global, print, attn.sig);
}