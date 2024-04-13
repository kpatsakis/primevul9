static bool gfs2_select_rgrp(struct gfs2_rgrpd **pos, const struct gfs2_rgrpd *begin)
{
	struct gfs2_rgrpd *rgd = *pos;
	struct gfs2_sbd *sdp = rgd->rd_sbd;

	rgd = gfs2_rgrpd_get_next(rgd);
	if (rgd == NULL)
		rgd = gfs2_rgrpd_get_first(sdp);
	*pos = rgd;
	if (rgd != begin) /* If we didn't wrap */
		return true;
	return false;
}