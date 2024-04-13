void gfs2_rs_deltree(struct gfs2_blkreserv *rs)
{
	struct gfs2_rgrpd *rgd;

	rgd = rs->rs_rbm.rgd;
	if (rgd) {
		spin_lock(&rgd->rd_rsspin);
		__rs_deltree(rs);
		spin_unlock(&rgd->rd_rsspin);
	}
}