socket_get_fd(context *ctx, int sd, int *fd)
{
	struct msghdr msg;
	struct iovec iov;
	char buf[2];

	size_t controllen = CMSG_SPACE(sizeof(int));
	struct cmsghdr *cm = malloc(controllen);
	if (!cm) {
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			"unable to allocate memory: %m");
		exit(1);
	}

	memset(&msg, '\0', sizeof(msg));
	iov.iov_base = buf;
	iov.iov_len = 2;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = cm;
	msg.msg_controllen = controllen;

	ssize_t n;
	n = recvmsg(sd, &msg, MSG_WAITALL);
	if (n < 0) {
malformed:
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			"unlock-token: invalid data");
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			"possible exploit attempt. closing.");
		close(sd);
		return;
	}

	struct cmsghdr *cme = CMSG_FIRSTHDR(&msg);

	if (cme == NULL)
		goto malformed;

	if (cme->cmsg_level != SOL_SOCKET)
		goto malformed;

	if (cme->cmsg_type != SCM_RIGHTS)
		goto malformed;

	*fd = *((int *)CMSG_DATA(cme));

	free(cm);
}