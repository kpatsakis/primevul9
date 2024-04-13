rdpdr_send_client_announce_reply(void)
{
	/* DR_CORE_CLIENT_ANNOUNCE_RSP */
	STREAM s;
	s = channel_init(rdpdr_channel, 12);
	out_uint16_le(s, RDPDR_CTYP_CORE);
	out_uint16_le(s, PAKID_CORE_CLIENTID_CONFIRM);
	out_uint16_le(s, 1);	/* VersionMajor, MUST be set to 0x1 */
	out_uint16_le(s, 5);	/* VersionMinor */
	out_uint32_be(s, g_client_id);	/* ClientID */
	s_mark_end(s);
	channel_send(s, rdpdr_channel);
}