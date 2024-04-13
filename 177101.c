static u32 gfs2_orlov_skip(const struct gfs2_inode *ip)
{
	const struct gfs2_sbd *sdp = GFS2_SB(&ip->i_inode);
	u32 skip;

	get_random_bytes(&skip, sizeof(skip));
	return skip % sdp->sd_rgrps;
}