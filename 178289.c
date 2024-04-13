static GIOChannel *get_listener(int family, int protocol, int index)
{
	GIOChannel *channel;
	const char *proto;
	union {
		struct sockaddr sa;
		struct sockaddr_in6 sin6;
		struct sockaddr_in sin;
	} s;
	socklen_t slen;
	int sk, type;
	char *interface;

	debug("family %d protocol %d index %d", family, protocol, index);

	switch (protocol) {
	case IPPROTO_UDP:
		proto = "UDP";
		type = SOCK_DGRAM | SOCK_CLOEXEC;
		break;

	case IPPROTO_TCP:
		proto = "TCP";
		type = SOCK_STREAM | SOCK_CLOEXEC;
		break;

	default:
		return NULL;
	}

	sk = socket(family, type, protocol);
	if (sk < 0 && family == AF_INET6 && errno == EAFNOSUPPORT) {
		connman_error("No IPv6 support");
		return NULL;
	}

	if (sk < 0) {
		connman_error("Failed to create %s listener socket", proto);
		return NULL;
	}

	interface = connman_inet_ifname(index);
	if (!interface || setsockopt(sk, SOL_SOCKET, SO_BINDTODEVICE,
					interface,
					strlen(interface) + 1) < 0) {
		connman_error("Failed to bind %s listener interface "
			"for %s (%d/%s)",
			proto, family == AF_INET ? "IPv4" : "IPv6",
			-errno, strerror(errno));
		close(sk);
		g_free(interface);
		return NULL;
	}
	g_free(interface);

	if (family == AF_INET6) {
		memset(&s.sin6, 0, sizeof(s.sin6));
		s.sin6.sin6_family = AF_INET6;
		s.sin6.sin6_port = htons(53);
		slen = sizeof(s.sin6);

		if (__connman_inet_get_interface_address(index,
						AF_INET6,
						&s.sin6.sin6_addr) < 0) {
			/* So we could not find suitable IPv6 address for
			 * the interface. This could happen if we have
			 * disabled IPv6 for the interface.
			 */
			close(sk);
			return NULL;
		}

	} else if (family == AF_INET) {
		memset(&s.sin, 0, sizeof(s.sin));
		s.sin.sin_family = AF_INET;
		s.sin.sin_port = htons(53);
		slen = sizeof(s.sin);

		if (__connman_inet_get_interface_address(index,
						AF_INET,
						&s.sin.sin_addr) < 0) {
			close(sk);
			return NULL;
		}
	} else {
		close(sk);
		return NULL;
	}

	if (bind(sk, &s.sa, slen) < 0) {
		connman_error("Failed to bind %s listener socket", proto);
		close(sk);
		return NULL;
	}

	if (protocol == IPPROTO_TCP) {

		if (listen(sk, 10) < 0) {
			connman_error("Failed to listen on TCP socket %d/%s",
				-errno, strerror(errno));
			close(sk);
			return NULL;
		}

		fcntl(sk, F_SETFL, O_NONBLOCK);
	}

	channel = g_io_channel_unix_new(sk);
	if (!channel) {
		connman_error("Failed to create %s listener channel", proto);
		close(sk);
		return NULL;
	}

	g_io_channel_set_close_on_unref(channel, TRUE);

	return channel;
}