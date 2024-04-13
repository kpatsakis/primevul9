static int legacy_reconfigure(struct fs_context *fc)
{
	struct legacy_fs_context *ctx = fc->fs_private;
	struct super_block *sb = fc->root->d_sb;

	if (!sb->s_op->remount_fs)
		return 0;

	return sb->s_op->remount_fs(sb, &fc->sb_flags,
				    ctx ? ctx->legacy_data : NULL);
}