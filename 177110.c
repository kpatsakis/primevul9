static void gfs2_set_alloc_start(struct gfs2_rbm *rbm,
				 const struct gfs2_inode *ip, bool dinode)
{
	u64 goal;

	if (gfs2_rs_active(&ip->i_res)) {
		*rbm = ip->i_res.rs_rbm;
		return;
	}

	if (!dinode && rgrp_contains_block(rbm->rgd, ip->i_goal))
		goal = ip->i_goal;
	else
		goal = rbm->rgd->rd_last_alloc + rbm->rgd->rd_data0;

	gfs2_rbm_from_block(rbm, goal);
}