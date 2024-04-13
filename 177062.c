int gfs2_alloc_blocks(struct gfs2_inode *ip, u64 *bn, unsigned int *nblocks,
		      bool dinode, u64 *generation)
{
	struct gfs2_sbd *sdp = GFS2_SB(&ip->i_inode);
	struct buffer_head *dibh;
	struct gfs2_rbm rbm = { .rgd = ip->i_rgd, };
	unsigned int ndata;
	u64 block; /* block, within the file system scope */
	int error;

	gfs2_set_alloc_start(&rbm, ip, dinode);
	error = gfs2_rbm_find(&rbm, GFS2_BLKST_FREE, NULL, ip, false);

	if (error == -ENOSPC) {
		gfs2_set_alloc_start(&rbm, ip, dinode);
		error = gfs2_rbm_find(&rbm, GFS2_BLKST_FREE, NULL, NULL, false);
	}

	/* Since all blocks are reserved in advance, this shouldn't happen */
	if (error) {
		fs_warn(sdp, "inum=%llu error=%d, nblocks=%u, full=%d fail_pt=%d\n",
			(unsigned long long)ip->i_no_addr, error, *nblocks,
			test_bit(GBF_FULL, &rbm.rgd->rd_bits->bi_flags),
			rbm.rgd->rd_extfail_pt);
		goto rgrp_error;
	}

	gfs2_alloc_extent(&rbm, dinode, nblocks);
	block = gfs2_rbm_to_block(&rbm);
	rbm.rgd->rd_last_alloc = block - rbm.rgd->rd_data0;
	if (gfs2_rs_active(&ip->i_res))
		gfs2_adjust_reservation(ip, &rbm, *nblocks);
	ndata = *nblocks;
	if (dinode)
		ndata--;

	if (!dinode) {
		ip->i_goal = block + ndata - 1;
		error = gfs2_meta_inode_buffer(ip, &dibh);
		if (error == 0) {
			struct gfs2_dinode *di =
				(struct gfs2_dinode *)dibh->b_data;
			gfs2_trans_add_meta(ip->i_gl, dibh);
			di->di_goal_meta = di->di_goal_data =
				cpu_to_be64(ip->i_goal);
			brelse(dibh);
		}
	}
	if (rbm.rgd->rd_free < *nblocks) {
		pr_warn("nblocks=%u\n", *nblocks);
		goto rgrp_error;
	}

	rbm.rgd->rd_free -= *nblocks;
	if (dinode) {
		rbm.rgd->rd_dinodes++;
		*generation = rbm.rgd->rd_igeneration++;
		if (*generation == 0)
			*generation = rbm.rgd->rd_igeneration++;
	}

	gfs2_trans_add_meta(rbm.rgd->rd_gl, rbm.rgd->rd_bits[0].bi_bh);
	gfs2_rgrp_out(rbm.rgd, rbm.rgd->rd_bits[0].bi_bh->b_data);
	gfs2_rgrp_ondisk2lvb(rbm.rgd->rd_rgl, rbm.rgd->rd_bits[0].bi_bh->b_data);

	gfs2_statfs_change(sdp, 0, -(s64)*nblocks, dinode ? 1 : 0);
	if (dinode)
		gfs2_trans_add_unrevoke(sdp, block, *nblocks);

	gfs2_quota_change(ip, *nblocks, ip->i_inode.i_uid, ip->i_inode.i_gid);

	rbm.rgd->rd_free_clone -= *nblocks;
	trace_gfs2_block_alloc(ip, rbm.rgd, block, *nblocks,
			       dinode ? GFS2_BLKST_DINODE : GFS2_BLKST_USED);
	*bn = block;
	return 0;

rgrp_error:
	gfs2_rgrp_error(rbm.rgd);
	return -EIO;
}