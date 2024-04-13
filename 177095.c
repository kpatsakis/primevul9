static int gfs2_ri_update(struct gfs2_inode *ip)
{
	struct gfs2_sbd *sdp = GFS2_SB(&ip->i_inode);
	int error;

	do {
		error = read_rindex_entry(ip);
	} while (error == 0);

	if (error < 0)
		return error;

	set_rgrp_preferences(sdp);

	sdp->sd_rindex_uptodate = 1;
	return 0;
}