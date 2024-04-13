int gg_image_request(struct gg_session *sess, uin_t recipient, int size, uint32_t crc32)
{
	struct gg_send_msg s;
	struct gg_msg_image_request r;
	char dummy = 0;
	int res;

	gg_debug_session(sess, GG_DEBUG_FUNCTION, "** gg_image_request(%p, %d, %u, 0x%.4x);\n", sess, recipient, size, crc32);

	if (!sess) {
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

	s.recipient = gg_fix32(recipient);
	s.seq = gg_fix32(0);
	s.msgclass = gg_fix32(GG_CLASS_MSG);

	r.flag = GG_MSG_OPTION_IMAGE_REQUEST;
	r.size = gg_fix32(size);
	r.crc32 = gg_fix32(crc32);

	res = gg_send_packet(sess, GG_SEND_MSG, &s, sizeof(s), &dummy, 1, &r, sizeof(r), NULL);

	if (!res) {
		struct gg_image_queue *q = malloc(sizeof(*q));
		char *buf;

		if (!q) {
			gg_debug_session(sess, GG_DEBUG_MISC, "// gg_image_request() not enough memory for image queue\n");
			return -1;
		}

		buf = malloc(size);
		if (size && !buf)
		{
			gg_debug_session(sess, GG_DEBUG_MISC, "// gg_image_request() not enough memory for image\n");
			free(q);
			return -1;
		}

		memset(q, 0, sizeof(*q));

		q->sender = recipient;
		q->size = size;
		q->crc32 = crc32;
		q->image = buf;

		if (!sess->images)
			sess->images = q;
		else {
			struct gg_image_queue *qq;

			for (qq = sess->images; qq->next; qq = qq->next)
				;

			qq->next = q;
		}
	}

	return res;
}