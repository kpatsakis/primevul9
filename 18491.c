handle_is_token_unlocked(context *ctx, struct pollfd *pollfd, socklen_t size)
{
	struct msghdr msg;
	struct iovec iov;
	ssize_t n;

	int rc = cms_context_alloc(&ctx->cms);
	if (rc < 0) {
		send_response(ctx, ctx->backup_cms, pollfd, rc);
		return;
	}

	steal_from_cms(ctx->backup_cms, ctx->cms);

	char *buffer = malloc(size);
	if (!buffer) {
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			"unable to allocate memory: %m");
		exit(1);
	}

	memset(&msg, '\0', sizeof(msg));

	iov.iov_base = buffer;
	iov.iov_len = size;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	n = recvmsg(pollfd->fd, &msg, MSG_WAITALL);

	pesignd_string *tn = (pesignd_string *)buffer;
	if (n < (long long)sizeof(tn->size)) {
malformed:
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			"unlock-token: invalid data");
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			"possible exploit attempt. closing.");
		close(pollfd->fd);
		return;
	}
	n -= sizeof(tn->size);
	if ((size_t)n < tn->size)
		goto malformed;
	n -= tn->size;

	if (tn->value[tn->size - 1] != '\0')
		goto malformed;

	if (n != 0)
		goto malformed;

	ctx->cms->log(ctx->cms, ctx->priority|LOG_NOTICE,
		"querying token \"%s\"", tn->value);

	char *key = (char *)tn->value;
	char *tokenname;

	tokenname = bsearch(&key, ctx->tokennames, ctx->ntokennames,
				sizeof (char *), cmpstringp);
	send_response(ctx, ctx->cms, pollfd, tokenname == NULL ? 1 : 0);

	ctx->cms->log(ctx->cms, ctx->priority|LOG_NOTICE,
			"token \"%s\" is %sunlocked", tn->value,
			tokenname == NULL ? "not " : "");

	free(buffer);

	hide_stolen_goods_from_cms(ctx->cms, ctx->backup_cms);
	cms_context_fini(ctx->cms);
}