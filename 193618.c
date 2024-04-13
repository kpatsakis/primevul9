int gg_add_notify_ex(struct gg_session *sess, uin_t uin, char type)
{
	gg_debug_session(sess, GG_DEBUG_FUNCTION, "** gg_add_notify_ex(%p, %u, %d);\n", sess, uin, type);

	if (!sess) {
		errno = EFAULT;
		return -1;
	}

	if (sess->state != GG_STATE_CONNECTED) {
		errno = ENOTCONN;
		return -1;
	}

	if (sess->protocol_version >= GG_PROTOCOL_110) {
		gg_tvbuilder_t *tvb = gg_tvbuilder_new(sess, NULL);
		gg_tvbuilder_expected_size(tvb, 16);

		gg_tvbuilder_write_uin(tvb, uin, 0);
		gg_tvbuilder_write_uint8(tvb, type);

		if (!gg_tvbuilder_send(tvb, GG_ADD_NOTIFY105))
			return -1;
		return 0;
	} else {
		struct gg_add_remove a;

		a.uin = gg_fix32(uin);
		a.dunno1 = type;

		return gg_send_packet(sess, GG_ADD_NOTIFY, &a, sizeof(a), NULL);
	}
}