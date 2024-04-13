int gg_write(struct gg_session *sess, const char *buf, int length)
{
	int res = 0;

	if (!sess->async) {
		int written = 0;

		while (written < length) {
			res = gg_write_common(sess, buf + written, length - written);

			if (res == -1)
				return -1;

			written += res;
			res = written;
		}
	} else {
		if (sess->send_buf == NULL) {
			res = gg_write_common(sess, buf, length);

			if (res == -1 && errno == EAGAIN)
				res = 0;
			if (res == -1)
				return -1;
		}

		if (res < length) {
			char *tmp;

			if (!(tmp = realloc(sess->send_buf, sess->send_left + length - res))) {
				errno = ENOMEM;
				return -1;
			}

			sess->send_buf = tmp;

			memcpy(sess->send_buf + sess->send_left, buf + res, length - res);

			sess->send_left += length - res;
		}
	}

	return res;
}