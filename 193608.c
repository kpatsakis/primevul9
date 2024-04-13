int gg_userlist100_request(struct gg_session *sess, char type, unsigned int version, char format_type, const char *request)
{
	struct gg_userlist100_request pkt;
	unsigned char *zrequest;
	size_t zrequest_len;
	int ret;

	if (!sess) {
		errno = EFAULT;
		return -1;
	}

	if (sess->state != GG_STATE_CONNECTED) {
		errno = ENOTCONN;
		return -1;
	}

	pkt.type = type;
	pkt.version = gg_fix32(version);
	pkt.format_type = format_type;
	pkt.unknown1 = 0x01;

	if (request == NULL)
		return gg_send_packet(sess, GG_USERLIST100_REQUEST, &pkt, sizeof(pkt), NULL);

	zrequest = gg_deflate(request, &zrequest_len);

	if (zrequest == NULL) {
		gg_debug_session(sess, GG_DEBUG_MISC, "// gg_userlist100_request() gg_deflate() failed\n");
		return -1;
	}

	ret = gg_send_packet(sess, GG_USERLIST100_REQUEST, &pkt, sizeof(pkt), zrequest, zrequest_len, NULL);

	free(zrequest);

	return ret;
}