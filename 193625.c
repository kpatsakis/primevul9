int gg_chat_create(struct gg_session *gs)
{
	struct gg_chat_create pkt;
	int seq = ++gs->seq;

	pkt.seq = gg_fix32(seq);
	pkt.dummy = 0;

	if (gg_send_packet(gs, GG_CHAT_CREATE, &pkt, sizeof(pkt), NULL) == -1)
		return -1;

	return seq;
}