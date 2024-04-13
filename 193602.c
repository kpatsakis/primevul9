int gg_typing_notification(struct gg_session *sess, uin_t recipient, int length){
	struct gg_typing_notification pkt;
	uin_t uin;

	pkt.length = gg_fix16(length);
	uin = gg_fix32(recipient);
	memcpy(&pkt.uin, &uin, sizeof(uin_t));

	return gg_send_packet(sess, GG_TYPING_NOTIFICATION, &pkt, sizeof(pkt), NULL);
}