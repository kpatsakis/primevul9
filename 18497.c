handle_signing(context *ctx, struct pollfd *pollfd, socklen_t size,
	       int attached, bool with_file_type)
{
	struct msghdr msg;
	struct iovec iov;
	ssize_t n;
	char *buffer = malloc(size);
	uint32_t file_format;

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

	if (with_file_type) {
		file_format = *((uint32_t *) buffer);
		n -= sizeof(uint32_t);
	} else {
		file_format = FORMAT_PE_BINARY;
	}

	pesignd_string *tn = (pesignd_string *)(buffer + sizeof(uint32_t));
	if (n < (long long)sizeof(tn->size)) {
malformed:
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			"handle_signing: invalid data");
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			"possible exploit attempt. closing.");
		close(pollfd->fd);
		return;
	}

	n -= sizeof(tn->size);
	if ((size_t)n < tn->size)
		goto malformed;
	n -= tn->size;

	/* authenticating with nss frees these ... best API ever. */
	ctx->cms->tokenname = PORT_ArenaStrdup(ctx->cms->arena,
						(char *)tn->value);
	if (!ctx->cms->tokenname)
		goto oom;

	if ((size_t)n < sizeof(tn->size))
		goto malformed;
	pesignd_string *cn = pesignd_string_next(tn);
	n -= sizeof(cn->size);
	if ((size_t)n < cn->size)
		goto malformed;

	ctx->cms->certname = PORT_ArenaStrdup(ctx->cms->arena,
						(char *)cn->value);
	if (!ctx->cms->certname)
		goto oom;

	n -= cn->size;
	if (n != 0)
		goto malformed;

	int infd=-1;
	socket_get_fd(ctx, pollfd->fd, &infd);

	int outfd=-1;
	socket_get_fd(ctx, pollfd->fd, &outfd);

	ctx->cms->log(ctx->cms, ctx->priority|LOG_NOTICE,
		"attempting to sign with key \"%s:%s\"",
		tn->value, cn->value);
	free(buffer);

	int rc = find_certificate(ctx->cms, 1);
	if (rc < 0) {
		goto finish;
	}

	switch (file_format) {
	case FORMAT_PE_BINARY:
		rc = sign_pe(ctx, infd, outfd, attached);
		break;
	case FORMAT_KERNEL_MODULE:
		rc = sign_kmod(ctx, infd, outfd, attached);
		break;
	default:
		rc = -1;
		break;
	}

	if (rc < 0)
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			      "unrecognised format %d", file_format);

finish:
	close(infd);
	close(outfd);

	send_response(ctx, ctx->cms, pollfd, rc);
	teardown_digests(ctx->cms);
}