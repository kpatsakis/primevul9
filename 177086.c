static void rg_mblk_search(struct gfs2_rgrpd *rgd, struct gfs2_inode *ip,
			   const struct gfs2_alloc_parms *ap)
{
	struct gfs2_rbm rbm = { .rgd = rgd, };
	u64 goal;
	struct gfs2_blkreserv *rs = &ip->i_res;
	u32 extlen;
	u32 free_blocks = rgd->rd_free_clone - rgd->rd_reserved;
	int ret;
	struct inode *inode = &ip->i_inode;

	if (S_ISDIR(inode->i_mode))
		extlen = 1;
	else {
		extlen = max_t(u32, atomic_read(&rs->rs_sizehint), ap->target);
		extlen = clamp(extlen, RGRP_RSRV_MINBLKS, free_blocks);
	}
	if ((rgd->rd_free_clone < rgd->rd_reserved) || (free_blocks < extlen))
		return;

	/* Find bitmap block that contains bits for goal block */
	if (rgrp_contains_block(rgd, ip->i_goal))
		goal = ip->i_goal;
	else
		goal = rgd->rd_last_alloc + rgd->rd_data0;

	if (WARN_ON(gfs2_rbm_from_block(&rbm, goal)))
		return;

	ret = gfs2_rbm_find(&rbm, GFS2_BLKST_FREE, &extlen, ip, true);
	if (ret == 0) {
		rs->rs_rbm = rbm;
		rs->rs_free = extlen;
		rs->rs_inum = ip->i_no_addr;
		rs_insert(ip);
	} else {
		if (goal == rgd->rd_last_alloc + rgd->rd_data0)
			rgd->rd_last_alloc = 0;
	}
}