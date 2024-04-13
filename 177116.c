static void gfs2_alloc_extent(const struct gfs2_rbm *rbm, bool dinode,
			     unsigned int *n)
{
	struct gfs2_rbm pos = { .rgd = rbm->rgd, };
	const unsigned int elen = *n;
	u64 block;
	int ret;

	*n = 1;
	block = gfs2_rbm_to_block(rbm);
	gfs2_trans_add_meta(rbm->rgd->rd_gl, rbm_bi(rbm)->bi_bh);
	gfs2_setbit(rbm, true, dinode ? GFS2_BLKST_DINODE : GFS2_BLKST_USED);
	block++;
	while (*n < elen) {
		ret = gfs2_rbm_from_block(&pos, block);
		if (ret || gfs2_testbit(&pos) != GFS2_BLKST_FREE)
			break;
		gfs2_trans_add_meta(pos.rgd->rd_gl, rbm_bi(&pos)->bi_bh);
		gfs2_setbit(&pos, true, GFS2_BLKST_USED);
		(*n)++;
		block++;
	}
}