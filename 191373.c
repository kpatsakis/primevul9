ssh_exchange_identification(int timeout_ms)
{
	char buf[256], remote_version[256];	/* must be same size! */
	int remote_major, remote_minor, mismatch;
	int connection_in = packet_get_connection_in();
	int connection_out = packet_get_connection_out();
	int minor1 = PROTOCOL_MINOR_1, client_banner_sent = 0;
	u_int i, n;
	size_t len;
	int fdsetsz, remaining, rc;
	struct timeval t_start, t_remaining;
	fd_set *fdset;

	fdsetsz = howmany(connection_in + 1, NFDBITS) * sizeof(fd_mask);
	fdset = xcalloc(1, fdsetsz);

	/*
	 * If we are SSH2-only then we can send the banner immediately and
	 * save a round-trip.
	 */
	if (options.protocol == SSH_PROTO_2) {
		enable_compat20();
		send_client_banner(connection_out, 0);
		client_banner_sent = 1;
	}

	/* Read other side's version identification. */
	remaining = timeout_ms;
	for (n = 0;;) {
		for (i = 0; i < sizeof(buf) - 1; i++) {
			if (timeout_ms > 0) {
				gettimeofday(&t_start, NULL);
				ms_to_timeval(&t_remaining, remaining);
				FD_SET(connection_in, fdset);
				rc = select(connection_in + 1, fdset, NULL,
				    fdset, &t_remaining);
				ms_subtract_diff(&t_start, &remaining);
				if (rc == 0 || remaining <= 0)
					fatal("Connection timed out during "
					    "banner exchange");
				if (rc == -1) {
					if (errno == EINTR)
						continue;
					fatal("ssh_exchange_identification: "
					    "select: %s", strerror(errno));
				}
			}

			len = roaming_atomicio(read, connection_in, &buf[i], 1);

			if (len != 1 && errno == EPIPE)
				fatal("ssh_exchange_identification: "
				    "Connection closed by remote host");
			else if (len != 1)
				fatal("ssh_exchange_identification: "
				    "read: %.100s", strerror(errno));
			if (buf[i] == '\r') {
				buf[i] = '\n';
				buf[i + 1] = 0;
				continue;		/**XXX wait for \n */
			}
			if (buf[i] == '\n') {
				buf[i + 1] = 0;
				break;
			}
			if (++n > 65536)
				fatal("ssh_exchange_identification: "
				    "No banner received");
		}
		buf[sizeof(buf) - 1] = 0;
		if (strncmp(buf, "SSH-", 4) == 0)
			break;
		debug("ssh_exchange_identification: %s", buf);
	}
	server_version_string = xstrdup(buf);
	free(fdset);

	/*
	 * Check that the versions match.  In future this might accept
	 * several versions and set appropriate flags to handle them.
	 */
	if (sscanf(server_version_string, "SSH-%d.%d-%[^\n]\n",
	    &remote_major, &remote_minor, remote_version) != 3)
		fatal("Bad remote protocol version identification: '%.100s'", buf);
	debug("Remote protocol version %d.%d, remote software version %.100s",
	    remote_major, remote_minor, remote_version);

	compat_datafellows(remote_version);
	mismatch = 0;

	switch (remote_major) {
	case 1:
		if (remote_minor == 99 &&
		    (options.protocol & SSH_PROTO_2) &&
		    !(options.protocol & SSH_PROTO_1_PREFERRED)) {
			enable_compat20();
			break;
		}
		if (!(options.protocol & SSH_PROTO_1)) {
			mismatch = 1;
			break;
		}
		if (remote_minor < 3) {
			fatal("Remote machine has too old SSH software version.");
		} else if (remote_minor == 3 || remote_minor == 4) {
			/* We speak 1.3, too. */
			enable_compat13();
			minor1 = 3;
			if (options.forward_agent) {
				logit("Agent forwarding disabled for protocol 1.3");
				options.forward_agent = 0;
			}
		}
		break;
	case 2:
		if (options.protocol & SSH_PROTO_2) {
			enable_compat20();
			break;
		}
		/* FALLTHROUGH */
	default:
		mismatch = 1;
		break;
	}
	if (mismatch)
		fatal("Protocol major versions differ: %d vs. %d",
		    (options.protocol & SSH_PROTO_2) ? PROTOCOL_MAJOR_2 : PROTOCOL_MAJOR_1,
		    remote_major);
	if ((datafellows & SSH_BUG_DERIVEKEY) != 0)
		fatal("Server version \"%.100s\" uses unsafe key agreement; "
		    "refusing connection", remote_version);
	if ((datafellows & SSH_BUG_RSASIGMD5) != 0)
		logit("Server version \"%.100s\" uses unsafe RSA signature "
		    "scheme; disabling use of RSA keys", remote_version);
	if (!client_banner_sent)
		send_client_banner(connection_out, minor1);
	chop(server_version_string);
}