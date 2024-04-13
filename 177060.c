void gfs2_inplace_release(struct gfs2_inode *ip)
{
	struct gfs2_blkreserv *rs = &ip->i_res;

	if (rs->rs_rgd_gh.gh_gl)
		gfs2_glock_dq_uninit(&rs->rs_rgd_gh);
}