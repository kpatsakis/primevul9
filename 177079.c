static bool gfs2_rgrp_used_recently(const struct gfs2_blkreserv *rs,
				    u64 msecs)
{
	u64 tdiff;

	tdiff = ktime_to_ns(ktime_sub(ktime_get_real(),
                            rs->rs_rbm.rgd->rd_gl->gl_dstamp));

	return tdiff > (msecs * 1000 * 1000);
}