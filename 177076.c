int gfs2_rindex_update(struct gfs2_sbd *sdp)
{
	struct gfs2_inode *ip = GFS2_I(sdp->sd_rindex);
	struct gfs2_glock *gl = ip->i_gl;
	struct gfs2_holder ri_gh;
	int error = 0;
	int unlock_required = 0;

	/* Read new copy from disk if we don't have the latest */
	if (!sdp->sd_rindex_uptodate) {
		if (!gfs2_glock_is_locked_by_me(gl)) {
			error = gfs2_glock_nq_init(gl, LM_ST_SHARED, 0, &ri_gh);
			if (error)
				return error;
			unlock_required = 1;
		}
		if (!sdp->sd_rindex_uptodate)
			error = gfs2_ri_update(ip);
		if (unlock_required)
			gfs2_glock_dq_uninit(&ri_gh);
	}

	return error;
}