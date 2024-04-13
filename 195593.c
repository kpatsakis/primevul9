static int ext4_quota_off(struct super_block *sb, int type)
{
	struct inode *inode = sb_dqopt(sb)->files[type];
	handle_t *handle;

	/* Force all delayed allocation blocks to be allocated.
	 * Caller already holds s_umount sem */
	if (test_opt(sb, DELALLOC))
		sync_filesystem(sb);

	if (!inode)
		goto out;

	/* Update modification times of quota files when userspace can
	 * start looking at them */
	handle = ext4_journal_start(inode, 1);
	if (IS_ERR(handle))
		goto out;
	inode->i_mtime = inode->i_ctime = CURRENT_TIME;
	ext4_mark_inode_dirty(handle, inode);
	ext4_journal_stop(handle);

out:
	return dquot_quota_off(sb, type);
}