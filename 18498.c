handle_get_cmd_version(context *ctx, struct pollfd *pollfd, socklen_t size)
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

	int32_t version = -1;
	uint32_t command;

	if (n < (long long)sizeof(command)) {
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			"unlock-token: invalid data");
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			"possible exploit attempt. closing.");
		close(pollfd->fd);
		return;
	}

	memcpy(&command, buffer, sizeof (command));
	ctx->cms->log(ctx->cms, ctx->priority|LOG_NOTICE,
			"searching for command %d", command);

	for (int i = 0; cmd_table[i].cmd != CMD_LIST_END; i++) {
		if (cmd_table[i].cmd == command) {
			ctx->cms->log(ctx->cms, ctx->priority|LOG_NOTICE,
					"cmd-version: found command \"%s\" "
					"version %d",
					cmd_table[i].name,
					cmd_table[i].version);
			version = cmd_table[i].version;
			break;
		}
	}

	if (version == -1) {
		ctx->cms->log(ctx->cms, ctx->priority|LOG_NOTICE,
				"cmd-version: could not find command %d",
				command);
	}
	send_response(ctx, ctx->cms, pollfd, version);

	free(buffer);

	hide_stolen_goods_from_cms(ctx->cms, ctx->backup_cms);
	cms_context_fini(ctx->cms);
}