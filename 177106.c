static u64 gfs2_next_unreserved_block(struct gfs2_rgrpd *rgd, u64 block,
				      u32 length,
				      const struct gfs2_inode *ip)
{
	struct gfs2_blkreserv *rs;
	struct rb_node *n;
	int rc;

	spin_lock(&rgd->rd_rsspin);
	n = rgd->rd_rstree.rb_node;
	while (n) {
		rs = rb_entry(n, struct gfs2_blkreserv, rs_node);
		rc = rs_cmp(block, length, rs);
		if (rc < 0)
			n = n->rb_left;
		else if (rc > 0)
			n = n->rb_right;
		else
			break;
	}

	if (n) {
		while ((rs_cmp(block, length, rs) == 0) && (&ip->i_res != rs)) {
			block = gfs2_rbm_to_block(&rs->rs_rbm) + rs->rs_free;
			n = n->rb_right;
			if (n == NULL)
				break;
			rs = rb_entry(n, struct gfs2_blkreserv, rs_node);
		}
	}

	spin_unlock(&rgd->rd_rsspin);
	return block;
}