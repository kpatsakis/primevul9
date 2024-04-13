struct fs_context *vfs_dup_fs_context(struct fs_context *src_fc)
{
	struct fs_context *fc;
	int ret;

	if (!src_fc->ops->dup)
		return ERR_PTR(-EOPNOTSUPP);

	fc = kmemdup(src_fc, sizeof(struct fs_context), GFP_KERNEL);
	if (!fc)
		return ERR_PTR(-ENOMEM);

	mutex_init(&fc->uapi_mutex);

	fc->fs_private	= NULL;
	fc->s_fs_info	= NULL;
	fc->source	= NULL;
	fc->security	= NULL;
	get_filesystem(fc->fs_type);
	get_net(fc->net_ns);
	get_user_ns(fc->user_ns);
	get_cred(fc->cred);
	if (fc->log.log)
		refcount_inc(&fc->log.log->usage);

	/* Can't call put until we've called ->dup */
	ret = fc->ops->dup(fc, src_fc);
	if (ret < 0)
		goto err_fc;

	ret = security_fs_context_dup(fc, src_fc);
	if (ret < 0)
		goto err_fc;
	return fc;

err_fc:
	put_fs_context(fc);
	return ERR_PTR(ret);
}