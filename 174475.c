mcs_send_cjrq(uint16 chanid)
{
	STREAM s;

	DEBUG_RDP5(("Sending CJRQ for channel #%d\n", chanid));

	s = iso_init(5);

	out_uint8(s, (MCS_CJRQ << 2));
	out_uint16_be(s, g_mcs_userid);
	out_uint16_be(s, chanid);

	s_mark_end(s);
	iso_send(s);
}