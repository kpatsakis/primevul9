int gg_image_reply(struct gg_session *sess, uin_t recipient, const char *filename, const char *image, int size)
{
	struct gg_msg_image_reply *r;
	struct gg_send_msg s;
	const char *tmp;
	char buf[1910];
	int res = -1;

	gg_debug_session(sess, GG_DEBUG_FUNCTION, "** gg_image_reply(%p, %d, \"%s\", %p, %d);\n", sess, recipient, filename, image, size);

	if (!sess || !filename || !image) {
		errno = EFAULT;
		return -1;
	}

	if (sess->state != GG_STATE_CONNECTED) {
		errno = ENOTCONN;
		return -1;
	}

	if (size < 0) {
		errno = EINVAL;
		return -1;
	}

	/* wytnij ścieżki, zostaw tylko nazwę pliku */
	while ((tmp = strrchr(filename, '/')) || (tmp = strrchr(filename, '\\')))
		filename = tmp + 1;

	if (strlen(filename) < 1 || strlen(filename) > 1024) {
		errno = EINVAL;
		return -1;
	}

	s.recipient = gg_fix32(recipient);
	s.seq = gg_fix32(0);
	s.msgclass = gg_fix32(GG_CLASS_MSG);

	buf[0] = 0;
	r = (void*) &buf[1];

	r->flag = GG_MSG_OPTION_IMAGE_REPLY;
	r->size = gg_fix32(size);
	r->crc32 = gg_fix32(gg_crc32(0, (const unsigned char*) image, size));

	while (size > 0) {
		size_t buflen, chunklen;

		/* \0 + struct gg_msg_image_reply */
		buflen = sizeof(struct gg_msg_image_reply) + 1;

		/* w pierwszym kawałku jest nazwa pliku */
		if (r->flag == GG_MSG_OPTION_IMAGE_REPLY) {
			strcpy(buf + buflen, filename);
			buflen += strlen(filename) + 1;
		}

		chunklen = ((size_t) size >= sizeof(buf) - buflen) ? (sizeof(buf) - buflen) : (size_t) size;

		memcpy(buf + buflen, image, chunklen);
		size -= chunklen;
		image += chunklen;

		res = gg_send_packet(sess, GG_SEND_MSG, &s, sizeof(s), buf, buflen + chunklen, NULL);

		if (res == -1)
			break;

		r->flag = GG_MSG_OPTION_IMAGE_REPLY_MORE;
	}

	return res;
}