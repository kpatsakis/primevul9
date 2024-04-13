void __gfs2_free_blocks(struct gfs2_inode *ip, u64 bstart, u32 blen, int meta)
{
	struct gfs2_sbd *sdp = GFS2_SB(&ip->i_inode);
	struct gfs2_rgrpd *rgd;

	rgd = rgblk_free(sdp, bstart, blen, GFS2_BLKST_FREE);
	if (!rgd)
		return;
	trace_gfs2_block_alloc(ip, rgd, bstart, blen, GFS2_BLKST_FREE);
	rgd->rd_free += blen;
	rgd->rd_flags &= ~GFS2_RGF_TRIMMED;
	gfs2_trans_add_meta(rgd->rd_gl, rgd->rd_bits[0].bi_bh);
	gfs2_rgrp_out(rgd, rgd->rd_bits[0].bi_bh->b_data);
	gfs2_rgrp_ondisk2lvb(rgd->rd_rgl, rgd->rd_bits[0].bi_bh->b_data);

	/* Directories keep their data in the metadata address space */
	if (meta || ip->i_depth)
		gfs2_meta_wipe(ip, bstart, blen);
}