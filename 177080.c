void gfs2_rlist_add(struct gfs2_inode *ip, struct gfs2_rgrp_list *rlist,
		    u64 block)
{
	struct gfs2_sbd *sdp = GFS2_SB(&ip->i_inode);
	struct gfs2_rgrpd *rgd;
	struct gfs2_rgrpd **tmp;
	unsigned int new_space;
	unsigned int x;

	if (gfs2_assert_warn(sdp, !rlist->rl_ghs))
		return;

	if (ip->i_rgd && rgrp_contains_block(ip->i_rgd, block))
		rgd = ip->i_rgd;
	else
		rgd = gfs2_blk2rgrpd(sdp, block, 1);
	if (!rgd) {
		fs_err(sdp, "rlist_add: no rgrp for block %llu\n", (unsigned long long)block);
		return;
	}
	ip->i_rgd = rgd;

	for (x = 0; x < rlist->rl_rgrps; x++)
		if (rlist->rl_rgd[x] == rgd)
			return;

	if (rlist->rl_rgrps == rlist->rl_space) {
		new_space = rlist->rl_space + 10;

		tmp = kcalloc(new_space, sizeof(struct gfs2_rgrpd *),
			      GFP_NOFS | __GFP_NOFAIL);

		if (rlist->rl_rgd) {
			memcpy(tmp, rlist->rl_rgd,
			       rlist->rl_space * sizeof(struct gfs2_rgrpd *));
			kfree(rlist->rl_rgd);
		}

		rlist->rl_space = new_space;
		rlist->rl_rgd = tmp;
	}

	rlist->rl_rgd[rlist->rl_rgrps++] = rgd;
}