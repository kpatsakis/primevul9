get_uid_and_gid(context *ctx, char **homedir)
{
	struct passwd *passwd;

	passwd = getpwnam("pesign");

	if (!passwd)
		return -1;

	ctx->uid = passwd->pw_uid;
	ctx->gid = passwd->pw_gid;

	if (ctx->uid == 0 || ctx->gid == 0) {
		ctx->backup_cms->log(ctx->backup_cms, ctx->priority|LOG_ERR,
			"cowardly refusing to start with uid = %d and gid = %d",
			ctx->uid, ctx->gid);
		errno = EINVAL;
		return -1;
	}

	*homedir = passwd->pw_dir;

	return 0;
}