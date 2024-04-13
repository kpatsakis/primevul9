struct gfs2_rgrpd *gfs2_rgrpd_get_next(struct gfs2_rgrpd *rgd)
{
	struct gfs2_sbd *sdp = rgd->rd_sbd;
	const struct rb_node *n;

	spin_lock(&sdp->sd_rindex_spin);
	n = rb_next(&rgd->rd_node);
	if (n == NULL)
		n = rb_first(&sdp->sd_rindex_tree);

	if (unlikely(&rgd->rd_node == n)) {
		spin_unlock(&sdp->sd_rindex_spin);
		return NULL;
	}
	rgd = rb_entry(n, struct gfs2_rgrpd, rd_node);
	spin_unlock(&sdp->sd_rindex_spin);
	return rgd;
}