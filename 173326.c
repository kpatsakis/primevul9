static void bnx2x_release_alr(struct bnx2x *bp)
{
	REG_WR(bp, MCP_REG_MCPR_ACCESS_LOCK, 0);
}