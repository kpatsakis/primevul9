int gfs2_fitrim(struct file *filp, void __user *argp)
{
	struct inode *inode = file_inode(filp);
	struct gfs2_sbd *sdp = GFS2_SB(inode);
	struct request_queue *q = bdev_get_queue(sdp->sd_vfs->s_bdev);
	struct buffer_head *bh;
	struct gfs2_rgrpd *rgd;
	struct gfs2_rgrpd *rgd_end;
	struct gfs2_holder gh;
	struct fstrim_range r;
	int ret = 0;
	u64 amt;
	u64 trimmed = 0;
	u64 start, end, minlen;
	unsigned int x;
	unsigned bs_shift = sdp->sd_sb.sb_bsize_shift;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (!blk_queue_discard(q))
		return -EOPNOTSUPP;

	if (copy_from_user(&r, argp, sizeof(r)))
		return -EFAULT;

	ret = gfs2_rindex_update(sdp);
	if (ret)
		return ret;

	start = r.start >> bs_shift;
	end = start + (r.len >> bs_shift);
	minlen = max_t(u64, r.minlen,
		       q->limits.discard_granularity) >> bs_shift;

	if (end <= start || minlen > sdp->sd_max_rg_data)
		return -EINVAL;

	rgd = gfs2_blk2rgrpd(sdp, start, 0);
	rgd_end = gfs2_blk2rgrpd(sdp, end, 0);

	if ((gfs2_rgrpd_get_first(sdp) == gfs2_rgrpd_get_next(rgd_end))
	    && (start > rgd_end->rd_data0 + rgd_end->rd_data))
		return -EINVAL; /* start is beyond the end of the fs */

	while (1) {

		ret = gfs2_glock_nq_init(rgd->rd_gl, LM_ST_EXCLUSIVE, 0, &gh);
		if (ret)
			goto out;

		if (!(rgd->rd_flags & GFS2_RGF_TRIMMED)) {
			/* Trim each bitmap in the rgrp */
			for (x = 0; x < rgd->rd_length; x++) {
				struct gfs2_bitmap *bi = rgd->rd_bits + x;
				ret = gfs2_rgrp_send_discards(sdp,
						rgd->rd_data0, NULL, bi, minlen,
						&amt);
				if (ret) {
					gfs2_glock_dq_uninit(&gh);
					goto out;
				}
				trimmed += amt;
			}

			/* Mark rgrp as having been trimmed */
			ret = gfs2_trans_begin(sdp, RES_RG_HDR, 0);
			if (ret == 0) {
				bh = rgd->rd_bits[0].bi_bh;
				rgd->rd_flags |= GFS2_RGF_TRIMMED;
				gfs2_trans_add_meta(rgd->rd_gl, bh);
				gfs2_rgrp_out(rgd, bh->b_data);
				gfs2_rgrp_ondisk2lvb(rgd->rd_rgl, bh->b_data);
				gfs2_trans_end(sdp);
			}
		}
		gfs2_glock_dq_uninit(&gh);

		if (rgd == rgd_end)
			break;

		rgd = gfs2_rgrpd_get_next(rgd);
	}

out:
	r.len = trimmed << bs_shift;
	if (copy_to_user(argp, &r, sizeof(r)))
		return -EFAULT;

	return ret;
}