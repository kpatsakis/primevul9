void gfs2_rsqa_delete(struct gfs2_inode *ip, atomic_t *wcount)
{
	down_write(&ip->i_rw_mutex);
	if ((wcount == NULL) || (atomic_read(wcount) <= 1)) {
		gfs2_rs_deltree(&ip->i_res);
		BUG_ON(ip->i_res.rs_free);
	}
	up_write(&ip->i_rw_mutex);
	gfs2_qa_delete(ip, wcount);
}