struct gfs2_rgrpd *gfs2_rgrpd_get_first(struct gfs2_sbd *sdp)
{
	const struct rb_node *n;
	struct gfs2_rgrpd *rgd;

	spin_lock(&sdp->sd_rindex_spin);
	n = rb_first(&sdp->sd_rindex_tree);
	rgd = rb_entry(n, struct gfs2_rgrpd, rd_node);
	spin_unlock(&sdp->sd_rindex_spin);

	return rgd;
}