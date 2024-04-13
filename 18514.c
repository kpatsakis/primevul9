handle_events(context *ctx)
{
	int rc;
	int nsockets = 1;

	struct pollfd *pollfds = calloc(1, sizeof(struct pollfd));

	if (!pollfds) {
		ctx->backup_cms->log(ctx->backup_cms, ctx->priority|LOG_ERR,
			"could not allocate memory: %m");
		exit(1);
	}

	pollfds[0].fd = ctx->sd;
	pollfds[0].events = POLLIN|POLLPRI|POLLHUP;

	while (1) {
		if (should_exit != 0) {
shutdown:
			do_shutdown(ctx, nsockets, pollfds);
			return 0;
		}
		rc = ppoll(pollfds, nsockets, NULL, NULL);
		if (should_exit != 0)
			goto shutdown;
		if (rc < 0) {
			ctx->backup_cms->log(ctx->backup_cms,
				ctx->priority|LOG_WARNING,
				"ppoll: %m");
			continue;
		}

		if (pollfds[0].revents & POLLIN) {
			nsockets++;
			struct pollfd *newpollfds = realloc(pollfds,
				nsockets * sizeof(struct pollfd));

			if (!newpollfds) {
				ctx->backup_cms->log(ctx->backup_cms,
					ctx->priority|LOG_ERR,
					"could not allocate memory: %m");
				exit(1);
			}
			pollfds = newpollfds;

			struct sockaddr_un remote;
			socklen_t len = sizeof(remote);
			pollfds[nsockets-1].fd = accept(pollfds[0].fd, &remote,
							&len);
			pollfds[nsockets-1].events = POLLIN|POLLPRI|POLLHUP;
			pollfds[nsockets-1].revents = pollfds[0].revents;
		}
		for (int i = 1; i < nsockets; i++) {
			if (pollfds[i].revents & (POLLHUP|POLLNVAL)) {
				close(pollfds[i].fd);
				if (i == nsockets-1) {
					nsockets--;
					continue;
				}
				for (int j = i; j < nsockets - 1; j++) {
					pollfds[j].fd = pollfds[j+1].fd;
					pollfds[j].events =
						pollfds[j].events;
					pollfds[j].revents =
						pollfds[j].revents;
				}
				nsockets--;
				i--;
				continue;
			}

			if (pollfds[i].revents & (POLLIN|POLLPRI))
				handle_event(ctx, &pollfds[i]);
		}
	}
	return 0;
}