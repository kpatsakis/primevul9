static int ext4_statfs_project(struct super_block *sb,
			       kprojid_t projid, struct kstatfs *buf)
{
	struct kqid qid;
	struct dquot *dquot;
	u64 limit;
	u64 curblock;

	qid = make_kqid_projid(projid);
	dquot = dqget(sb, qid);
	if (IS_ERR(dquot))
		return PTR_ERR(dquot);
	spin_lock(&dquot->dq_dqb_lock);

	limit = (dquot->dq_dqb.dqb_bsoftlimit ?
		 dquot->dq_dqb.dqb_bsoftlimit :
		 dquot->dq_dqb.dqb_bhardlimit) >> sb->s_blocksize_bits;
	if (limit && buf->f_blocks > limit) {
		curblock = dquot->dq_dqb.dqb_curspace >> sb->s_blocksize_bits;
		buf->f_blocks = limit;
		buf->f_bfree = buf->f_bavail =
			(buf->f_blocks > curblock) ?
			 (buf->f_blocks - curblock) : 0;
	}

	limit = dquot->dq_dqb.dqb_isoftlimit ?
		dquot->dq_dqb.dqb_isoftlimit :
		dquot->dq_dqb.dqb_ihardlimit;
	if (limit && buf->f_files > limit) {
		buf->f_files = limit;
		buf->f_ffree =
			(buf->f_files > dquot->dq_dqb.dqb_curinodes) ?
			 (buf->f_files - dquot->dq_dqb.dqb_curinodes) : 0;
	}

	spin_unlock(&dquot->dq_dqb_lock);
	dqput(dquot);
	return 0;
}