handle_unlock_token(context *ctx, struct pollfd *pollfd, socklen_t size)
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
oom:
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

	pesignd_string *tp = pesignd_string_next(tn);
	if ((size_t)n < sizeof(tp->size))
		goto malformed;
	n -= sizeof(tp->size);
	if ((size_t)n < tp->size)
		goto malformed;
	n -= tp->size;

	if (tn->value[tn->size - 1] != '\0')
		goto malformed;

	if (n != 0)
		goto malformed;

	ctx->cms->log(ctx->cms, ctx->priority|LOG_NOTICE,
		"unlocking token \"%s\"", tn->value);

	/* authenticating with nss frees this ... best API ever. */
	ctx->cms->tokenname = PORT_ArenaStrdup(ctx->cms->arena,
						(char *)tn->value);
	if (!ctx->cms->tokenname)
		goto oom;

	char *pin = (char *)tp->value;
	if (!pin)
		goto oom;

	secuPWData pwdata;

	memset(&pwdata, 0, sizeof(pwdata));
	pwdata.source = pwdata.orig_source = PW_PLAINTEXT;
	pwdata.data = pin;

	cms_set_pw_callback(ctx->cms, get_password_passthrough);
	cms_set_pw_data(ctx->cms, &pwdata);

	rc = unlock_nss_token(ctx->cms);

	cms_set_pw_callback(ctx->cms, get_password_fail);
	cms_set_pw_data(ctx->cms, NULL);

	if (rc == -1)
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			"could not find token \"%s\"", tn->value);
	else if (rc == 0) {
		ctx->cms->log(ctx->cms, ctx->priority|LOG_NOTICE,
			"authentication succeeded for token \"%s\"",
			tn->value);
		rc = add_token_to_authenticated_list(ctx, tn->value);
		if (rc < 0)
			ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
				"couldn't add token to internal list: %m");
	}

	send_response(ctx, ctx->cms, pollfd, rc);
	free(buffer);

	hide_stolen_goods_from_cms(ctx->cms, ctx->backup_cms);
	cms_context_fini(ctx->cms);
}