rdpdr_send_client_name_request(void)
{
	/* DR_CORE_CLIENT_NAME_REQ */
	STREAM s;
	uint32 hostlen;

	if (NULL == g_rdpdr_clientname)
	{
		g_rdpdr_clientname = g_hostname;
	}
	hostlen = (strlen(g_rdpdr_clientname) + 1) * 2;

	s = channel_init(rdpdr_channel, 16 + hostlen);
	out_uint16_le(s, RDPDR_CTYP_CORE);
	out_uint16_le(s, PAKID_CORE_CLIENT_NAME);
	out_uint32_le(s, 1);	/* UnicodeFlag */
	out_uint32_le(s, 0);	/* CodePage */
	out_uint32_le(s, hostlen);	/* ComputerNameLen */
	rdp_out_unistr(s, g_rdpdr_clientname, hostlen - 2);
	s_mark_end(s);
	channel_send(s, rdpdr_channel);
}