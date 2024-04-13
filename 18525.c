handle_event(context *ctx, struct pollfd *pollfd)
{
	struct msghdr msg;
	struct iovec iov;
	ssize_t n;
	pesignd_msghdr pm;

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov.iov_base = &pm;
	iov.iov_len = sizeof(pm);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	char control[1024];
	msg.msg_controllen = 1024;
	msg.msg_control = control;

	n = recvmsg(pollfd->fd, &msg, MSG_WAITALL);
	if (n < 0) {
		ctx->backup_cms->log(ctx->backup_cms, ctx->priority|LOG_WARNING,
			"recvmsg failed: %m");
		return n;
	}

	/* if recvmsg returned 0, we're not going to get any valid data. */
	/* This *probably* means we were hung up on. */
	if (n == 0)
		return n;

	if (n < (long long)sizeof (pm)) {
		ctx->backup_cms->log(ctx->backup_cms, ctx->priority|LOG_ERR,
			"got message with invalid size %zu", n);
		ctx->backup_cms->log(ctx->backup_cms, ctx->priority|LOG_ERR,
			"possible exploit attempt.  closing.");
		close(pollfd->fd);
		return -1;
	}

	if (pm.version != PESIGND_VERSION) {
		ctx->backup_cms->log(ctx->backup_cms, ctx->priority|LOG_ERR,
			"got version %#x, expected version %#x",
			pm.version, PESIGND_VERSION);
		ctx->backup_cms->log(ctx->backup_cms, ctx->priority|LOG_ERR,
			"possible exploit attempt.  closing.");
		close(pollfd->fd);
		return -1;
	}

	for (int i = 0; cmd_table[i].cmd != CMD_LIST_END; i++) {
		if (cmd_table[i].cmd == pm.command) {
			if (cmd_table[i].func == NULL) {
				handle_invalid_input(pm.command, ctx, pollfd,
							pm.size);
				close(pollfd->fd);
			}
			cmd_table[i].func(ctx, pollfd, pm.size);
			return 0;
		}
	}

	handle_invalid_input(pm.command, ctx, pollfd, pm.size);
	close(pollfd->fd);
	return 0;
}