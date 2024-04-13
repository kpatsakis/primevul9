int gg_notify_ex(struct gg_session *sess, uin_t *userlist, char *types, int count)
{
	struct gg_notify *n;
	uin_t *u;
	char *t;
	int i, res = 0;

	gg_debug_session(sess, GG_DEBUG_FUNCTION, "** gg_notify_ex(%p, %p, %p, %d);\n", sess, userlist, types, count);

	if (!sess) {
		errno = EFAULT;
		return -1;
	}

	if (sess->state != GG_STATE_CONNECTED) {
		errno = ENOTCONN;
		return -1;
	}

	if (sess->protocol_version >= GG_PROTOCOL_110)
		return gg_notify105_ex(sess, userlist, types, count);

	if (!userlist || !count)
		return gg_send_packet(sess, GG_LIST_EMPTY, NULL);

	while (count > 0) {
		int part_count, packet_type;

		if (count > 400) {
			part_count = 400;
			packet_type = GG_NOTIFY_FIRST;
		} else {
			part_count = count;
			packet_type = GG_NOTIFY_LAST;
		}

		if (!(n = (struct gg_notify*) malloc(sizeof(*n) * part_count)))
			return -1;

		for (u = userlist, t = types, i = 0; i < part_count; u++, t++, i++) {
			n[i].uin = gg_fix32(*u);
			if (types == NULL)
				n[i].dunno1 = GG_USER_NORMAL;
			else
				n[i].dunno1 = *t;
		}

		if (gg_send_packet(sess, packet_type, n, sizeof(*n) * part_count, NULL) == -1) {
			free(n);
			res = -1;
			break;
		}

		count -= part_count;
		userlist += part_count;
		if (types != NULL)
			types += part_count;

		free(n);
	}

	return res;
}