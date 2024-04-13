static int ext4_acquire_dquot(struct dquot *dquot)
{
	int ret, err;
	handle_t *handle;

	handle = ext4_journal_start(dquot_to_inode(dquot),
				    EXT4_QUOTA_INIT_BLOCKS(dquot->dq_sb));
	if (IS_ERR(handle))
		return PTR_ERR(handle);
	ret = dquot_acquire(dquot);
	err = ext4_journal_stop(handle);
	if (!ret)
		ret = err;
	return ret;
}