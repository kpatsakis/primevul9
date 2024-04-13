int gg_read(struct gg_session *sess, char *buf, int length)
{
	int res;

#ifdef GG_CONFIG_HAVE_GNUTLS
	if (sess->ssl != NULL) {
		for (;;) {
			res = gnutls_record_recv(GG_SESSION_GNUTLS(sess), buf, length);

			if (res < 0) {
				if (res == GNUTLS_E_AGAIN)
					errno = EAGAIN;
				else if (!gnutls_error_is_fatal(res) || res == GNUTLS_E_INTERRUPTED)
					continue;
				else
					errno = EINVAL;

				return -1;
			}

			return res;
		}
	}
#endif

#ifdef GG_CONFIG_HAVE_OPENSSL
	if (sess->ssl != NULL) {
		for (;;) {
			int err;

			res = SSL_read(sess->ssl, buf, length);

			if (res < 0) {
				err = SSL_get_error(sess->ssl, res);

				if (err == SSL_ERROR_SYSCALL && errno == EINTR)
					continue;

				if (err == SSL_ERROR_WANT_READ)
					errno = EAGAIN;
				else if (err != SSL_ERROR_SYSCALL)
					errno = EINVAL;

				return -1;
			}

			return res;
		}
	}
#endif

	for (;;) {
		res = recv(sess->fd, buf, length, 0);

		if (res == -1 && errno == EINTR)
			continue;

		return res;
	}
}