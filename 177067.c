void gfs2_unlink_di(struct inode *inode)
{
	struct gfs2_inode *ip = GFS2_I(inode);
	struct gfs2_sbd *sdp = GFS2_SB(inode);
	struct gfs2_rgrpd *rgd;
	u64 blkno = ip->i_no_addr;

	rgd = rgblk_free(sdp, blkno, 1, GFS2_BLKST_UNLINKED);
	if (!rgd)
		return;
	trace_gfs2_block_alloc(ip, rgd, blkno, 1, GFS2_BLKST_UNLINKED);
	gfs2_trans_add_meta(rgd->rd_gl, rgd->rd_bits[0].bi_bh);
	gfs2_rgrp_out(rgd, rgd->rd_bits[0].bi_bh->b_data);
	gfs2_rgrp_ondisk2lvb(rgd->rd_rgl, rgd->rd_bits[0].bi_bh->b_data);
	update_rgrp_lvb_unlinked(rgd, 1);
}