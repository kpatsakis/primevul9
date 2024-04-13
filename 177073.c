void check_and_update_goal(struct gfs2_inode *ip)
{
	struct gfs2_sbd *sdp = GFS2_SB(&ip->i_inode);
	if (!ip->i_goal || gfs2_blk2rgrpd(sdp, ip->i_goal, 1) == NULL)
		ip->i_goal = ip->i_no_addr;
}