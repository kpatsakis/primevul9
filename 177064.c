static bool gfs2_unaligned_extlen(struct gfs2_rbm *rbm, u32 n_unaligned, u32 *len)
{
	u32 n;
	u8 res;

	for (n = 0; n < n_unaligned; n++) {
		res = gfs2_testbit(rbm);
		if (res != GFS2_BLKST_FREE)
			return true;
		(*len)--;
		if (*len == 0)
			return true;
		if (gfs2_rbm_incr(rbm))
			return true;
	}

	return false;
}