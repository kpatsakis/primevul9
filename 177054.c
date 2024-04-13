static unsigned char gfs2_get_block_type(struct gfs2_rgrpd *rgd, u64 block)
{
	struct gfs2_rbm rbm = { .rgd = rgd, };
	int ret;

	ret = gfs2_rbm_from_block(&rbm, block);
	WARN_ON_ONCE(ret != 0);

	return gfs2_testbit(&rbm);
}