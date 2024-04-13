static void bnx2x_init_searcher(struct bnx2x *bp)
{
	int port = BP_PORT(bp);
	bnx2x_src_init_t2(bp, bp->t2, bp->t2_mapping, SRC_CONN_NUM);
	/* T1 hash bits value determines the T1 number of entries */
	REG_WR(bp, SRC_REG_NUMBER_HASH_BITS0 + port*4, SRC_HASH_BITS);
}