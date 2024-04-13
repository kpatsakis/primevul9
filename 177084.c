static bool gfs2_rgrp_congested(const struct gfs2_rgrpd *rgd, int loops)
{
	const struct gfs2_glock *gl = rgd->rd_gl;
	const struct gfs2_sbd *sdp = gl->gl_name.ln_sbd;
	struct gfs2_lkstats *st;
	u64 r_dcount, l_dcount;
	u64 l_srttb, a_srttb = 0;
	s64 srttb_diff;
	u64 sqr_diff;
	u64 var;
	int cpu, nonzero = 0;

	preempt_disable();
	for_each_present_cpu(cpu) {
		st = &per_cpu_ptr(sdp->sd_lkstats, cpu)->lkstats[LM_TYPE_RGRP];
		if (st->stats[GFS2_LKS_SRTTB]) {
			a_srttb += st->stats[GFS2_LKS_SRTTB];
			nonzero++;
		}
	}
	st = &this_cpu_ptr(sdp->sd_lkstats)->lkstats[LM_TYPE_RGRP];
	if (nonzero)
		do_div(a_srttb, nonzero);
	r_dcount = st->stats[GFS2_LKS_DCOUNT];
	var = st->stats[GFS2_LKS_SRTTVARB] +
	      gl->gl_stats.stats[GFS2_LKS_SRTTVARB];
	preempt_enable();

	l_srttb = gl->gl_stats.stats[GFS2_LKS_SRTTB];
	l_dcount = gl->gl_stats.stats[GFS2_LKS_DCOUNT];

	if ((l_dcount < 1) || (r_dcount < 1) || (a_srttb == 0))
		return false;

	srttb_diff = a_srttb - l_srttb;
	sqr_diff = srttb_diff * srttb_diff;

	var *= 2;
	if (l_dcount < 8 || r_dcount < 8)
		var *= 2;
	if (loops == 1)
		var *= 2;

	return ((srttb_diff < 0) && (sqr_diff > var));
}