static void bnx2x_prev_unload_undi_inc(struct bnx2x *bp, u8 inc)
{
	u16 rcq, bd;
	u32 addr, tmp_reg;

	if (BP_FUNC(bp) < 2)
		addr = BNX2X_PREV_UNDI_PROD_ADDR(BP_PORT(bp));
	else
		addr = BNX2X_PREV_UNDI_PROD_ADDR_H(BP_FUNC(bp) - 2);

	tmp_reg = REG_RD(bp, addr);
	rcq = BNX2X_PREV_UNDI_RCQ(tmp_reg) + inc;
	bd = BNX2X_PREV_UNDI_BD(tmp_reg) + inc;

	tmp_reg = BNX2X_PREV_UNDI_PROD(rcq, bd);
	REG_WR(bp, addr, tmp_reg);

	BNX2X_DEV_INFO("UNDI producer [%d/%d][%08x] rings bd -> 0x%04x, rcq -> 0x%04x\n",
		       BP_PORT(bp), BP_FUNC(bp), addr, bd, rcq);
}