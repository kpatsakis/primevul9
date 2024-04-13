static void ext4_write_super(struct super_block *sb)
{
	lock_super(sb);
	ext4_commit_super(sb, 1);
	unlock_super(sb);
}