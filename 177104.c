void gfs2_rgrp_dump(struct seq_file *seq, const struct gfs2_glock *gl)
{
	struct gfs2_rgrpd *rgd = gl->gl_object;
	struct gfs2_blkreserv *trs;
	const struct rb_node *n;

	if (rgd == NULL)
		return;
	gfs2_print_dbg(seq, " R: n:%llu f:%02x b:%u/%u i:%u r:%u e:%u\n",
		       (unsigned long long)rgd->rd_addr, rgd->rd_flags,
		       rgd->rd_free, rgd->rd_free_clone, rgd->rd_dinodes,
		       rgd->rd_reserved, rgd->rd_extfail_pt);
	spin_lock(&rgd->rd_rsspin);
	for (n = rb_first(&rgd->rd_rstree); n; n = rb_next(&trs->rs_node)) {
		trs = rb_entry(n, struct gfs2_blkreserv, rs_node);
		dump_rs(seq, trs);
	}
	spin_unlock(&rgd->rd_rsspin);
}