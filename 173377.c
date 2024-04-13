static void bnx2x_e1h_disable(struct bnx2x *bp)
{
	int port = BP_PORT(bp);

	bnx2x_tx_disable(bp);

	REG_WR(bp, NIG_REG_LLH0_FUNC_EN + port*8, 0);
}