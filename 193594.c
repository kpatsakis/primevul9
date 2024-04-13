struct gg_session *gg_login(const struct gg_login_params *p)
{
	struct gg_session *sess = NULL;

	if (p == NULL) {
		gg_debug(GG_DEBUG_FUNCTION, "** gg_login(%p);\n", p);
		errno = EFAULT;
		return NULL;
	}

	gg_debug(GG_DEBUG_FUNCTION, "** gg_login(%p: [uin=%u, async=%d, ...]);\n", p, p->uin, p->async);

	sess = malloc(sizeof(struct gg_session));

	if (sess == NULL) {
		gg_debug(GG_DEBUG_MISC, "// gg_login() not enough memory for session data\n");
		goto fail;
	}

	memset(sess, 0, sizeof(struct gg_session));

	if (p->password == NULL || p->uin == 0) {
		gg_debug(GG_DEBUG_MISC, "// gg_login() invalid arguments. uin and password needed\n");
		errno = EFAULT;
		goto fail;
	}

	if (!(sess->password = strdup(p->password))) {
		gg_debug(GG_DEBUG_MISC, "// gg_login() not enough memory for password\n");
		goto fail;
	}

	if (p->hash_type < 0 || p->hash_type > GG_LOGIN_HASH_SHA1) {
		gg_debug(GG_DEBUG_MISC, "// gg_login() invalid arguments. unknown hash type (%d)\n", p->hash_type);
		errno = EFAULT;
		goto fail;
	}

	sess->uin = p->uin;
	sess->state = GG_STATE_RESOLVING;
	sess->check = GG_CHECK_READ;
	sess->timeout = GG_DEFAULT_TIMEOUT;
	sess->async = p->async;
	sess->type = GG_SESSION_GG;
	sess->initial_status = p->status;
	sess->callback = gg_session_callback;
	sess->destroy = gg_free_session;
	sess->port = p->server_port;
	sess->server_addr = p->server_addr;
	sess->external_port = p->external_port;
	sess->external_addr = p->external_addr;
	sess->client_addr = p->client_addr;
	sess->client_port = p->client_port;

	if (p->protocol_features == 0) {
		sess->protocol_features = GG_FEATURE_MSG80 | GG_FEATURE_STATUS80 | GG_FEATURE_DND_FFC | GG_FEATURE_IMAGE_DESCR | GG_FEATURE_UNKNOWN_100 | GG_FEATURE_USER_DATA | GG_FEATURE_MSG_ACK | GG_FEATURE_TYPING_NOTIFICATION;
	} else {
		sess->protocol_features = (p->protocol_features & ~(GG_FEATURE_STATUS77 | GG_FEATURE_MSG77));

		if (!(p->protocol_features & GG_FEATURE_STATUS77))
			sess->protocol_features |= GG_FEATURE_STATUS80;

		if (!(p->protocol_features & GG_FEATURE_MSG77))
			sess->protocol_features |= GG_FEATURE_MSG80;
	}

	if (!(sess->status_flags = p->status_flags))
		sess->status_flags = GG_STATUS_FLAG_UNKNOWN | GG_STATUS_FLAG_SPAM;

	if (!p->protocol_version)
		sess->protocol_version = GG_DEFAULT_PROTOCOL_VERSION;
	else if (p->protocol_version < 0x2e) {
		gg_debug(GG_DEBUG_MISC, "// gg_login() libgadu no longer support protocol < 0x2e\n");
		sess->protocol_version = 0x2e;
	} else
		sess->protocol_version = p->protocol_version;

	sess->client_version = (p->client_version) ? strdup(p->client_version) : NULL;
	sess->last_sysmsg = p->last_sysmsg;
	sess->image_size = p->image_size;
	sess->pid = -1;
	sess->encoding = p->encoding;

	if (gg_session_set_resolver(sess, p->resolver) == -1) {
		gg_debug(GG_DEBUG_MISC, "// gg_login() invalid arguments. unsupported resolver type (%d)\n", p->resolver);
		errno = EFAULT;
		goto fail;
	}

	if (p->status_descr) {
		sess->initial_descr = gg_encoding_convert(p->status_descr, p->encoding, GG_ENCODING_UTF8, -1, -1);

		if (!sess->initial_descr) {
			gg_debug(GG_DEBUG_MISC, "// gg_login() not enough memory for status\n");
			goto fail;
		}
		
		/* XXX pamiętać, żeby nie ciąć w środku znaku utf-8 */
		
		if (strlen(sess->initial_descr) > GG_STATUS_DESCR_MAXSIZE)
			sess->initial_descr[GG_STATUS_DESCR_MAXSIZE] = 0;
	}

	if (p->tls != GG_SSL_DISABLED) {
#if !defined(GG_CONFIG_HAVE_GNUTLS) && !defined(GG_CONFIG_HAVE_OPENSSL)
		gg_debug(GG_DEBUG_MISC, "// gg_login() client requested TLS but no support compiled in\n");

		if (p->tls == GG_SSL_REQUIRED) {
			errno = ENOSYS;
			goto fail;
		}
#else
		sess->ssl_flag = p->tls;
#endif
	}

	if (p->hash_type)
		sess->hash_type = p->hash_type;
	else
		sess->hash_type = GG_LOGIN_HASH_SHA1;

	if (sess->server_addr == 0) {
		if (gg_proxy_enabled) {
			sess->resolver_host = gg_proxy_host;
			sess->proxy_port = gg_proxy_port;
			sess->state = (sess->async) ? GG_STATE_RESOLVE_PROXY_HUB_ASYNC : GG_STATE_RESOLVE_PROXY_HUB_SYNC;
		} else {
			sess->resolver_host = GG_APPMSG_HOST;
			sess->proxy_port = 0;
			sess->state = (sess->async) ? GG_STATE_RESOLVE_HUB_ASYNC : GG_STATE_RESOLVE_HUB_SYNC;
		}
	} else {
		// XXX inet_ntoa i wielowątkowość
		sess->connect_host = strdup(inet_ntoa(*(struct in_addr*) &sess->server_addr));
		if (sess->connect_host == NULL)
			goto fail;
		sess->connect_index = 0;

		if (gg_proxy_enabled) {
			sess->resolver_host = gg_proxy_host;
			sess->proxy_port = gg_proxy_port;
			if (sess->port == 0)
				sess->connect_port[0] = GG_HTTPS_PORT;
			else
				sess->connect_port[0] = sess->port;
			sess->connect_port[1] = 0;
			sess->state = (sess->async) ? GG_STATE_RESOLVE_PROXY_GG_ASYNC : GG_STATE_RESOLVE_PROXY_GG_SYNC;
		} else {
			sess->resolver_host = sess->connect_host;
			if (sess->port == 0) {
				if (sess->ssl_flag == GG_SSL_DISABLED) {
					sess->connect_port[0] = GG_DEFAULT_PORT;
					sess->connect_port[1] = GG_HTTPS_PORT;
				} else {
					sess->connect_port[0] = GG_HTTPS_PORT;
					sess->connect_port[1] = 0;
				}
			} else {
				sess->connect_port[0] = sess->port;
				sess->connect_port[1] = 0;
			}
			sess->state = (sess->async) ? GG_STATE_RESOLVE_GG_ASYNC : GG_STATE_RESOLVE_GG_SYNC;
		}
	}

	// XXX inaczej gg_watch_fd() wyjdzie z timeoutem
	sess->timeout = GG_DEFAULT_TIMEOUT;

	if (!sess->async) {
		while (!GG_SESSION_IS_CONNECTED(sess)) {
			struct gg_event *ge;

			ge = gg_watch_fd(sess);

			if (ge == NULL) {
				gg_debug(GG_DEBUG_MISC, "// gg_session_connect() critical error in gg_watch_fd()\n");
				goto fail;
			}

			if (ge->type == GG_EVENT_CONN_FAILED) {
				errno = EACCES;
				gg_debug(GG_DEBUG_MISC, "// gg_session_connect() could not login\n");
				gg_event_free(ge);
				goto fail;
			}

			gg_event_free(ge);
		}
	} else {
		struct gg_event *ge;

		ge = gg_watch_fd(sess);

		if (ge == NULL) {
			gg_debug(GG_DEBUG_MISC, "// gg_session_connect() critical error in gg_watch_fd()\n");
			goto fail;
		}

		gg_event_free(ge);
	}

	

	return sess;

fail:
	gg_free_session(sess);

	return NULL;
}