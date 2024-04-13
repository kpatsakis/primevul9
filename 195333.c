void fc_drop_locked(struct fs_context *fc)
{
	struct super_block *sb = fc->root->d_sb;
	dput(fc->root);
	fc->root = NULL;
	deactivate_locked_super(sb);
}