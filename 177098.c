int gfs2_rgrp_go_lock(struct gfs2_holder *gh)
{
	struct gfs2_rgrpd *rgd = gh->gh_gl->gl_object;
	struct gfs2_sbd *sdp = rgd->rd_sbd;

	if (gh->gh_flags & GL_SKIP && sdp->sd_args.ar_rgrplvb)
		return 0;
	return gfs2_rgrp_bh_get(rgd);
}