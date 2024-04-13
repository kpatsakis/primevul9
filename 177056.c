struct gfs2_rgrpd *gfs2_blk2rgrpd(struct gfs2_sbd *sdp, u64 blk, bool exact)
{
	struct rb_node *n, *next;
	struct gfs2_rgrpd *cur;

	spin_lock(&sdp->sd_rindex_spin);
	n = sdp->sd_rindex_tree.rb_node;
	while (n) {
		cur = rb_entry(n, struct gfs2_rgrpd, rd_node);
		next = NULL;
		if (blk < cur->rd_addr)
			next = n->rb_left;
		else if (blk >= cur->rd_data0 + cur->rd_data)
			next = n->rb_right;
		if (next == NULL) {
			spin_unlock(&sdp->sd_rindex_spin);
			if (exact) {
				if (blk < cur->rd_addr)
					return NULL;
				if (blk >= cur->rd_data0 + cur->rd_data)
					return NULL;
			}
			return cur;
		}
		n = next;
	}
	spin_unlock(&sdp->sd_rindex_spin);

	return NULL;
}