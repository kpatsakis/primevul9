u64 gfs2_ri_total(struct gfs2_sbd *sdp)
{
	u64 total_data = 0;	
	struct inode *inode = sdp->sd_rindex;
	struct gfs2_inode *ip = GFS2_I(inode);
	char buf[sizeof(struct gfs2_rindex)];
	int error, rgrps;

	for (rgrps = 0;; rgrps++) {
		loff_t pos = rgrps * sizeof(struct gfs2_rindex);

		if (pos + sizeof(struct gfs2_rindex) > i_size_read(inode))
			break;
		error = gfs2_internal_read(ip, buf, &pos,
					   sizeof(struct gfs2_rindex));
		if (error != sizeof(struct gfs2_rindex))
			break;
		total_data += be32_to_cpu(((struct gfs2_rindex *)buf)->ri_data);
	}
	return total_data;
}