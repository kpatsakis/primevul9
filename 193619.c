int gg_chat_invite(struct gg_session *gs, uint64_t id, uin_t *participants,
	unsigned int participants_count)
{
	struct gg_chat_invite pkt;
	int i, seq, ret;
	struct gg_chat_participant
	{
		uint32_t uin;
		uint32_t dummy;
	} GG_PACKED;
	struct gg_chat_participant *participants_list;
	size_t participants_list_size;

	if (participants_count == 0 || participants_count >= ~0 / sizeof(struct gg_chat_participant))
		return -1;

	participants_list_size = sizeof(struct gg_chat_participant) *
		participants_count;
	participants_list = malloc(participants_list_size);
	if (participants_list == NULL)
		return -1;

	seq = ++gs->seq;
	pkt.id = gg_fix64(id);
	pkt.seq = gg_fix32(seq);
	pkt.participants_count = gg_fix32(participants_count);

	for (i = 0; i < participants_count; i++)
	{
		participants_list[i].uin = gg_fix32(participants[i]);
		participants_list[i].dummy = gg_fix32(0x1e);
	}

	ret = gg_send_packet(gs, GG_CHAT_INVITE,
		&pkt, sizeof(pkt),
		participants_list, participants_list_size,
		NULL);
	free(participants_list);

	if (ret == -1)
		return -1;
	return seq;
}