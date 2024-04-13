static inline int rs_cmp(u64 blk, u32 len, struct gfs2_blkreserv *rs)
{
	u64 startblk = gfs2_rbm_to_block(&rs->rs_rbm);

	if (blk >= startblk + rs->rs_free)
		return 1;
	if (blk + len - 1 < startblk)
		return -1;
	return 0;
}