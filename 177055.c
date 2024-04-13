static void rs_insert(struct gfs2_inode *ip)
{
	struct rb_node **newn, *parent = NULL;
	int rc;
	struct gfs2_blkreserv *rs = &ip->i_res;
	struct gfs2_rgrpd *rgd = rs->rs_rbm.rgd;
	u64 fsblock = gfs2_rbm_to_block(&rs->rs_rbm);

	BUG_ON(gfs2_rs_active(rs));

	spin_lock(&rgd->rd_rsspin);
	newn = &rgd->rd_rstree.rb_node;
	while (*newn) {
		struct gfs2_blkreserv *cur =
			rb_entry(*newn, struct gfs2_blkreserv, rs_node);

		parent = *newn;
		rc = rs_cmp(fsblock, rs->rs_free, cur);
		if (rc > 0)
			newn = &((*newn)->rb_right);
		else if (rc < 0)
			newn = &((*newn)->rb_left);
		else {
			spin_unlock(&rgd->rd_rsspin);
			WARN_ON(1);
			return;
		}
	}

	rb_link_node(&rs->rs_node, parent, newn);
	rb_insert_color(&rs->rs_node, &rgd->rd_rstree);

	/* Do our rgrp accounting for the reservation */
	rgd->rd_reserved += rs->rs_free; /* blocks reserved */
	spin_unlock(&rgd->rd_rsspin);
	trace_gfs2_rs(rs, TRACE_RS_INSERT);
}