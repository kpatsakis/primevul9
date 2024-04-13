static int gg_write_common(struct gg_session *sess, const char *buf, int length)
{
	int res;

#ifdef GG_CONFIG_HAVE_GNUTLS
	if (sess->ssl != NULL) {
		for (;;) {
			res = gnutls_record_send(GG_SESSION_GNUTLS(sess), buf, length);

			if (res < 0) {
				if (!gnutls_error_is_fatal(res) || res == GNUTLS_E_INTERRUPTED)
					continue;

				if (res == GNUTLS_E_AGAIN)
					errno = EAGAIN;
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

			res = SSL_write(sess->ssl, buf, length);

			if (res < 0) {
				err = SSL_get_error(sess->ssl, res);

				if (err == SSL_ERROR_SYSCALL && errno == EINTR)
					continue;

				if (err == SSL_ERROR_WANT_WRITE)
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
		res = send(sess->fd, buf, length, 0);

		if (res == -1 && errno == EINTR)
			continue;

		return res;
	}
}