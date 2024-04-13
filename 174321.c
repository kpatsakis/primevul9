rdp_send_synchronise(void)
{
	STREAM s;

	s = rdp_init_data(4);

	out_uint16_le(s, 1);	/* type */
	out_uint16_le(s, 1002);

	s_mark_end(s);
	rdp_send_data(s, RDP_DATA_PDU_SYNCHRONISE);
}