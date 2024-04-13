static int legacy_init_fs_context(struct fs_context *fc)
{
	fc->fs_private = kzalloc(sizeof(struct legacy_fs_context), GFP_KERNEL_ACCOUNT);
	if (!fc->fs_private)
		return -ENOMEM;
	fc->ops = &legacy_fs_context_ops;
	return 0;
}