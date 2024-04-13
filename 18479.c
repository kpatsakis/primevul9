set_up_socket(context *ctx)
{
	int sd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sd < 0) {
		ctx->backup_cms->log(ctx->backup_cms, ctx->priority|LOG_ERR,
			"unable to create socket: %m");
		exit(1);
	}

	struct sockaddr_un addr_un = {
		.sun_family = AF_UNIX,
		.sun_path = SOCKPATH,
	};

	int rc = bind(sd, &addr_un, sizeof(addr_un));
	if (rc < 0) {
		ctx->backup_cms->log(ctx->backup_cms, ctx->priority|LOG_ERR,
			"unable to bind to \"%s\": %m",
			addr_un.sun_path);
		exit(1);
	}
	rc = chmod(SOCKPATH, 0660);
	if (rc < 0) {
		ctx->backup_cms->log(ctx->backup_cms, ctx->priority|LOG_ERR,
			"could not set permissions for \"%s\": %m",
			SOCKPATH);
		exit(1);
	}

	rc = listen(sd, 5);
	if (rc < 0) {
		ctx->backup_cms->log(ctx->backup_cms, ctx->priority|LOG_ERR,
			"unable to listen on socket: %m");
		exit(1);
	}


	ctx->sd = sd;
	return 0;
}