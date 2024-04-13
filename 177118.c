int gfs2_inplace_reserve(struct gfs2_inode *ip, struct gfs2_alloc_parms *ap)
{
	struct gfs2_sbd *sdp = GFS2_SB(&ip->i_inode);
	struct gfs2_rgrpd *begin = NULL;
	struct gfs2_blkreserv *rs = &ip->i_res;
	int error = 0, rg_locked, flags = 0;
	u64 last_unlinked = NO_BLOCK;
	int loops = 0;
	u32 skip = 0;

	if (sdp->sd_args.ar_rgrplvb)
		flags |= GL_SKIP;
	if (gfs2_assert_warn(sdp, ap->target))
		return -EINVAL;
	if (gfs2_rs_active(rs)) {
		begin = rs->rs_rbm.rgd;
	} else if (ip->i_rgd && rgrp_contains_block(ip->i_rgd, ip->i_goal)) {
		rs->rs_rbm.rgd = begin = ip->i_rgd;
	} else {
		check_and_update_goal(ip);
		rs->rs_rbm.rgd = begin = gfs2_blk2rgrpd(sdp, ip->i_goal, 1);
	}
	if (S_ISDIR(ip->i_inode.i_mode) && (ap->aflags & GFS2_AF_ORLOV))
		skip = gfs2_orlov_skip(ip);
	if (rs->rs_rbm.rgd == NULL)
		return -EBADSLT;

	while (loops < 3) {
		rg_locked = 1;

		if (!gfs2_glock_is_locked_by_me(rs->rs_rbm.rgd->rd_gl)) {
			rg_locked = 0;
			if (skip && skip--)
				goto next_rgrp;
			if (!gfs2_rs_active(rs)) {
				if (loops == 0 &&
				    !fast_to_acquire(rs->rs_rbm.rgd))
					goto next_rgrp;
				if ((loops < 2) &&
				    gfs2_rgrp_used_recently(rs, 1000) &&
				    gfs2_rgrp_congested(rs->rs_rbm.rgd, loops))
					goto next_rgrp;
			}
			error = gfs2_glock_nq_init(rs->rs_rbm.rgd->rd_gl,
						   LM_ST_EXCLUSIVE, flags,
						   &rs->rs_rgd_gh);
			if (unlikely(error))
				return error;
			if (!gfs2_rs_active(rs) && (loops < 2) &&
			    gfs2_rgrp_congested(rs->rs_rbm.rgd, loops))
				goto skip_rgrp;
			if (sdp->sd_args.ar_rgrplvb) {
				error = update_rgrp_lvb(rs->rs_rbm.rgd);
				if (unlikely(error)) {
					gfs2_glock_dq_uninit(&rs->rs_rgd_gh);
					return error;
				}
			}
		}

		/* Skip unuseable resource groups */
		if ((rs->rs_rbm.rgd->rd_flags & (GFS2_RGF_NOALLOC |
						 GFS2_RDF_ERROR)) ||
		    (loops == 0 && ap->target > rs->rs_rbm.rgd->rd_extfail_pt))
			goto skip_rgrp;

		if (sdp->sd_args.ar_rgrplvb)
			gfs2_rgrp_bh_get(rs->rs_rbm.rgd);

		/* Get a reservation if we don't already have one */
		if (!gfs2_rs_active(rs))
			rg_mblk_search(rs->rs_rbm.rgd, ip, ap);

		/* Skip rgrps when we can't get a reservation on first pass */
		if (!gfs2_rs_active(rs) && (loops < 1))
			goto check_rgrp;

		/* If rgrp has enough free space, use it */
		if (rs->rs_rbm.rgd->rd_free_clone >= ap->target ||
		    (loops == 2 && ap->min_target &&
		     rs->rs_rbm.rgd->rd_free_clone >= ap->min_target)) {
			ip->i_rgd = rs->rs_rbm.rgd;
			ap->allowed = ip->i_rgd->rd_free_clone;
			return 0;
		}
check_rgrp:
		/* Check for unlinked inodes which can be reclaimed */
		if (rs->rs_rbm.rgd->rd_flags & GFS2_RDF_CHECK)
			try_rgrp_unlink(rs->rs_rbm.rgd, &last_unlinked,
					ip->i_no_addr);
skip_rgrp:
		/* Drop reservation, if we couldn't use reserved rgrp */
		if (gfs2_rs_active(rs))
			gfs2_rs_deltree(rs);

		/* Unlock rgrp if required */
		if (!rg_locked)
			gfs2_glock_dq_uninit(&rs->rs_rgd_gh);
next_rgrp:
		/* Find the next rgrp, and continue looking */
		if (gfs2_select_rgrp(&rs->rs_rbm.rgd, begin))
			continue;
		if (skip)
			continue;

		/* If we've scanned all the rgrps, but found no free blocks
		 * then this checks for some less likely conditions before
		 * trying again.
		 */
		loops++;
		/* Check that fs hasn't grown if writing to rindex */
		if (ip == GFS2_I(sdp->sd_rindex) && !sdp->sd_rindex_uptodate) {
			error = gfs2_ri_update(ip);
			if (error)
				return error;
		}
		/* Flushing the log may release space */
		if (loops == 2)
			gfs2_log_flush(sdp, NULL, NORMAL_FLUSH);
	}

	return -ENOSPC;
}