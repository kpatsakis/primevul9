int gg_multilogon_disconnect(struct gg_session *gs, gg_multilogon_id_t conn_id)
{
	struct gg_multilogon_disconnect pkt;

	pkt.conn_id = conn_id;

	return gg_send_packet(gs, GG_MULTILOGON_DISCONNECT, &pkt, sizeof(pkt), NULL);
}