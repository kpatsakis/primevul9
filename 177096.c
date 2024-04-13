void gfs2_rgrp_go_unlock(struct gfs2_holder *gh)
{
	struct gfs2_rgrpd *rgd = gh->gh_gl->gl_object;
	int demote_requested = test_bit(GLF_DEMOTE, &gh->gh_gl->gl_flags) |
		test_bit(GLF_PENDING_DEMOTE, &gh->gh_gl->gl_flags);

	if (rgd && demote_requested)
		gfs2_rgrp_brelse(rgd);
}