static int ext4_unfreeze(struct super_block *sb)
{
	if (sb->s_flags & MS_RDONLY)
		return 0;

	lock_super(sb);
	/* Reset the needs_recovery flag before the fs is unlocked. */
	EXT4_SET_INCOMPAT_FEATURE(sb, EXT4_FEATURE_INCOMPAT_RECOVER);
	ext4_commit_super(sb, 1);
	unlock_super(sb);
	return 0;
}