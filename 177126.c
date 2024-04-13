static struct gfs2_rgrpd *rgblk_free(struct gfs2_sbd *sdp, u64 bstart,
				     u32 blen, unsigned char new_state)
{
	struct gfs2_rbm rbm;
	struct gfs2_bitmap *bi, *bi_prev = NULL;

	rbm.rgd = gfs2_blk2rgrpd(sdp, bstart, 1);
	if (!rbm.rgd) {
		if (gfs2_consist(sdp))
			fs_err(sdp, "block = %llu\n", (unsigned long long)bstart);
		return NULL;
	}

	gfs2_rbm_from_block(&rbm, bstart);
	while (blen--) {
		bi = rbm_bi(&rbm);
		if (bi != bi_prev) {
			if (!bi->bi_clone) {
				bi->bi_clone = kmalloc(bi->bi_bh->b_size,
						      GFP_NOFS | __GFP_NOFAIL);
				memcpy(bi->bi_clone + bi->bi_offset,
				       bi->bi_bh->b_data + bi->bi_offset,
				       bi->bi_len);
			}
			gfs2_trans_add_meta(rbm.rgd->rd_gl, bi->bi_bh);
			bi_prev = bi;
		}
		gfs2_setbit(&rbm, false, new_state);
		gfs2_rbm_incr(&rbm);
	}

	return rbm.rgd;
}