void *gg_recv_packet(struct gg_session *sess)
{
	struct gg_header *gh;
	char *packet;
	int res;
	size_t len;

	gg_debug_session(sess, GG_DEBUG_FUNCTION, "** gg_recv_packet(%p);\n", sess);

	if (sess == NULL) {
		errno = EFAULT;
		return NULL;
	}

	for (;;) {
		if (sess->recv_buf == NULL && sess->recv_done == 0) {
			sess->recv_buf = malloc(sizeof(struct gg_header) + 1);

			if (sess->recv_buf == NULL) {
				gg_debug_session(sess, GG_DEBUG_ERROR, "// gg_recv_packet() out of memory\n");
				return NULL;
			}
		}

		gh = (struct gg_header*) sess->recv_buf;

		if ((size_t) sess->recv_done < sizeof(struct gg_header)) {
			len = sizeof(struct gg_header) - sess->recv_done;
			gg_debug_session(sess, GG_DEBUG_NET, "// gg_recv_packet() header: %d done, %d to go\n", sess->recv_done, len);
		} else {
			if ((size_t) sess->recv_done >= sizeof(struct gg_header) + gg_fix32(gh->length)) {
				gg_debug_session(sess, GG_DEBUG_NET, "// gg_recv_packet() and that's it\n");
				break;
			}

			len = sizeof(struct gg_header) + gg_fix32(gh->length) - sess->recv_done;

			gg_debug_session(sess, GG_DEBUG_NET, "// gg_recv_packet() payload: %d done, %d length, %d to go\n", sess->recv_done, gg_fix32(gh->length), len);
		}

		res = gg_read(sess, sess->recv_buf + sess->recv_done, len);

		if (res == 0) {
			errno = ECONNRESET;
			gg_debug_session(sess, GG_DEBUG_ERROR, "// gg_recv_packet() connection broken\n");
			goto fail;
		}

		if (res == -1 && errno == EAGAIN) {
			gg_debug_session(sess, GG_DEBUG_NET, "// gg_recv_packet() resource temporarily unavailable\n");
			goto eagain;
		}

		if (res == -1) {
			gg_debug_session(sess, GG_DEBUG_ERROR, "// gg_recv_packet() read failed: errno=%d, %s\n", errno, strerror(errno));
			goto fail;
		}

		gg_debug_session(sess, GG_DEBUG_NET, "// gg_recv_packet() read %d bytes\n", res);

		if (sess->recv_done + res == sizeof(struct gg_header)) {
			char *tmp;

			gg_debug_session(sess, GG_DEBUG_NET, "// gg_recv_packet() header complete, payload %d bytes\n", gg_fix32(gh->length));

			if (gg_fix32(gh->length == 0))
				break;

			if (gg_fix32(gh->length) > 65535) {
				gg_debug_session(sess, GG_DEBUG_ERROR, "// gg_recv_packet() invalid packet length (%d)\n", gg_fix32(gh->length));
				errno = ERANGE;
				goto fail;
			}

			tmp = realloc(sess->recv_buf, sizeof(struct gg_header) + gg_fix32(gh->length) + 1);

			if (tmp == NULL) {
				gg_debug_session(sess, GG_DEBUG_ERROR, "// gg_recv_packet() out of memory\n");
				goto fail;
			}

			sess->recv_buf = tmp;
		}

		sess->recv_done += res;
	}

	packet = sess->recv_buf;
	sess->recv_buf = NULL;
	sess->recv_done = 0;

	/* Czasami zakładamy, że teksty w pakietach są zakończone zerem */
	packet[sizeof(struct gg_header) + gg_fix32(gh->length)] = 0;

	gg_debug_session(sess, GG_DEBUG_MISC, "// gg_recv_packet(type=0x%.2x, length=%d)\n", gg_fix32(gh->type), gg_fix32(gh->length));
	gg_debug_dump(sess, GG_DEBUG_DUMP, packet, sizeof(struct gg_header) + gg_fix32(gh->length));

	gh->type = gg_fix32(gh->type);
	gh->length = gg_fix32(gh->length);

	return packet;

fail:
	free(sess->recv_buf);
	sess->recv_buf = NULL;
	sess->recv_done = 0;

eagain:
	return NULL;
}