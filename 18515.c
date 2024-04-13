check_socket(context *ctx UNUSED)
{
	errno = 0;
	int rc = access(SOCKPATH, R_OK);
	if (rc == 0) {
		struct sockaddr_un addr_un = {
			.sun_family = AF_UNIX,
			.sun_path = SOCKPATH,
		};

		int sd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
		if (sd < 0) {
			fprintf(stderr, "unable to create socket: %m");
			exit(1);
		}

		socklen_t len = strlen(addr_un.sun_path) +
				sizeof(addr_un.sun_family);

		rc = connect(sd, (struct sockaddr *)&addr_un, len);
		if (rc < 0) {
			close(sd);
			unlink(SOCKPATH);
			return;
		}

		struct sockaddr_un remote;
		socklen_t size = sizeof(remote);
		rc = getpeername(sd, &remote, &size);
		if (rc < 0) {
			close(sd);
			return;
		} else {
			fprintf(stderr, "already running");
			exit(1);
		}
	} else {
		/* It could be something other than EEXIST, but it really
		 * doesn't matter since the daemon isn't running.  Blindly
		 * remove it. */
		unlink(SOCKPATH);
	}
}