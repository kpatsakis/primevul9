void put_fs_context(struct fs_context *fc)
{
	struct super_block *sb;

	if (fc->root) {
		sb = fc->root->d_sb;
		dput(fc->root);
		fc->root = NULL;
		deactivate_super(sb);
	}

	if (fc->need_free && fc->ops && fc->ops->free)
		fc->ops->free(fc);

	security_free_mnt_opts(&fc->security);
	put_net(fc->net_ns);
	put_user_ns(fc->user_ns);
	put_cred(fc->cred);
	put_fc_log(fc);
	put_filesystem(fc->fs_type);
	kfree(fc->source);
	kfree(fc);
}