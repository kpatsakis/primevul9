static int legacy_get_tree(struct fs_context *fc)
{
	struct legacy_fs_context *ctx = fc->fs_private;
	struct super_block *sb;
	struct dentry *root;

	root = fc->fs_type->mount(fc->fs_type, fc->sb_flags,
				      fc->source, ctx->legacy_data);
	if (IS_ERR(root))
		return PTR_ERR(root);

	sb = root->d_sb;
	BUG_ON(!sb);

	fc->root = root;
	return 0;
}