void vfs_clean_context(struct fs_context *fc)
{
	if (fc->need_free && fc->ops && fc->ops->free)
		fc->ops->free(fc);
	fc->need_free = false;
	fc->fs_private = NULL;
	fc->s_fs_info = NULL;
	fc->sb_flags = 0;
	security_free_mnt_opts(&fc->security);
	kfree(fc->source);
	fc->source = NULL;

	fc->purpose = FS_CONTEXT_FOR_RECONFIGURE;
	fc->phase = FS_CONTEXT_AWAITING_RECONF;
}