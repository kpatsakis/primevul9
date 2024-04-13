static void return_all_reservations(struct gfs2_rgrpd *rgd)
{
	struct rb_node *n;
	struct gfs2_blkreserv *rs;

	spin_lock(&rgd->rd_rsspin);
	while ((n = rb_first(&rgd->rd_rstree))) {
		rs = rb_entry(n, struct gfs2_blkreserv, rs_node);
		__rs_deltree(rs);
	}
	spin_unlock(&rgd->rd_rsspin);
}