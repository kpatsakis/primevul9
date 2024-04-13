int gg_chat_leave(struct gg_session *gs, uint64_t id)
{
	struct gg_chat_leave pkt;
	int seq = ++gs->seq;

	pkt.id = gg_fix64(id);
	pkt.seq = gg_fix32(seq);

	if (gg_send_packet(gs, GG_CHAT_LEAVE, &pkt, sizeof(pkt), NULL) == -1)
		return -1;

	return seq;
}