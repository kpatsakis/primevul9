void bnx2x_int_enable(struct bnx2x *bp)
{
	if (bp->common.int_block == INT_BLOCK_HC)
		bnx2x_hc_int_enable(bp);
	else
		bnx2x_igu_int_enable(bp);
}