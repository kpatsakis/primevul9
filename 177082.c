static inline int fast_to_acquire(struct gfs2_rgrpd *rgd)
{
	struct gfs2_glock *gl = rgd->rd_gl;

	if (gl->gl_state != LM_ST_UNLOCKED && list_empty(&gl->gl_holders) &&
	    !test_bit(GLF_DEMOTE_IN_PROGRESS, &gl->gl_flags) &&
	    !test_bit(GLF_DEMOTE, &gl->gl_flags))
		return 1;
	if (rgd->rd_flags & GFS2_RDF_PREFERRED)
		return 1;
	return 0;
}