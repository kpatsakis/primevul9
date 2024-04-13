static int f2fs_freeze(struct super_block *sb)
{
	int err;

	if (f2fs_readonly(sb))
		return 0;

	err = f2fs_sync_fs(sb, 1);
	return err;
}