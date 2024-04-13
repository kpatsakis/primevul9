static int ext4_quota_on(struct super_block *sb, int type, int format_id,
			 struct path *path)
{
	int err;

	if (!test_opt(sb, QUOTA))
		return -EINVAL;

	/* Quotafile not on the same filesystem? */
	if (path->dentry->d_sb != sb)
		return -EXDEV;
	/* Journaling quota? */
	if (EXT4_SB(sb)->s_qf_names[type]) {
		/* Quotafile not in fs root? */
		if (path->dentry->d_parent != sb->s_root)
			ext4_msg(sb, KERN_WARNING,
				"Quota file not on filesystem root. "
				"Journaled quota will not work");
	}

	/*
	 * When we journal data on quota file, we have to flush journal to see
	 * all updates to the file when we bypass pagecache...
	 */
	if (EXT4_SB(sb)->s_journal &&
	    ext4_should_journal_data(path->dentry->d_inode)) {
		/*
		 * We don't need to lock updates but journal_flush() could
		 * otherwise be livelocked...
		 */
		jbd2_journal_lock_updates(EXT4_SB(sb)->s_journal);
		err = jbd2_journal_flush(EXT4_SB(sb)->s_journal);
		jbd2_journal_unlock_updates(EXT4_SB(sb)->s_journal);
		if (err)
			return err;
	}

	return dquot_quota_on(sb, type, format_id, path);
}