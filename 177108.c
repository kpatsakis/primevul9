void gfs2_free_meta(struct gfs2_inode *ip, u64 bstart, u32 blen)
{
	struct gfs2_sbd *sdp = GFS2_SB(&ip->i_inode);

	__gfs2_free_blocks(ip, bstart, blen, 1);
	gfs2_statfs_change(sdp, 0, +blen, 0);
	gfs2_quota_change(ip, -(s64)blen, ip->i_inode.i_uid, ip->i_inode.i_gid);
}