send_response(context *ctx, cms_context *cms, struct pollfd *pollfd, int32_t rc)
{
	struct msghdr msg;
	struct iovec iov;
	ssize_t n;
	int msglen = ctx->errstr ? strlen(ctx->errstr) + 1 : 0;

	iov.iov_len = sizeof(pesignd_msghdr) + sizeof(pesignd_cmd_response)
			+ msglen;

	void *buffer = calloc(1, iov.iov_len);
	if (!buffer) {
		cms->log(cms, ctx->priority|LOG_ERR,
			"could not allocate memory: %m");
		exit(1);
	}

	iov.iov_base = buffer;

	pesignd_msghdr *pm = buffer;
	pesignd_cmd_response *resp = (pesignd_cmd_response *)((uint8_t *)pm +
					offsetof(pesignd_msghdr, size) +
					sizeof (pm->size));

	pm->version = PESIGND_VERSION;
	pm->command = CMD_RESPONSE;
	pm->size = sizeof(resp->rc) + msglen;

	memset(&msg, '\0', sizeof(msg));
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	resp->rc = rc;
	if (ctx->errstr)
		memcpy(resp->errmsg, ctx->errstr, msglen);

	n = sendmsg(pollfd->fd, &msg, 0);
	if (n < 0)
		cms->log(cms, ctx->priority|LOG_WARNING,
			"could not send response to client: %m");

	free(buffer);
}