static void bnx2x_int_disable(struct bnx2x *bp)
{
	if (bp->common.int_block == INT_BLOCK_HC)
		bnx2x_hc_int_disable(bp);
	else
		bnx2x_igu_int_disable(bp);
}