static void gfs2_free_uninit_di(struct gfs2_rgrpd *rgd, u64 blkno)
{
	struct gfs2_sbd *sdp = rgd->rd_sbd;
	struct gfs2_rgrpd *tmp_rgd;

	tmp_rgd = rgblk_free(sdp, blkno, 1, GFS2_BLKST_FREE);
	if (!tmp_rgd)
		return;
	gfs2_assert_withdraw(sdp, rgd == tmp_rgd);

	if (!rgd->rd_dinodes)
		gfs2_consist_rgrpd(rgd);
	rgd->rd_dinodes--;
	rgd->rd_free++;

	gfs2_trans_add_meta(rgd->rd_gl, rgd->rd_bits[0].bi_bh);
	gfs2_rgrp_out(rgd, rgd->rd_bits[0].bi_bh->b_data);
	gfs2_rgrp_ondisk2lvb(rgd->rd_rgl, rgd->rd_bits[0].bi_bh->b_data);
	update_rgrp_lvb_unlinked(rgd, -1);

	gfs2_statfs_change(sdp, 0, +1, -1);
}