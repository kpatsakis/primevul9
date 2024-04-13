rdp_send_fonts(uint16 seq)
{
	STREAM s;

	s = rdp_init_data(8);

	out_uint16(s, 0);	/* number of fonts */
	out_uint16_le(s, 0);	/* pad? */
	out_uint16_le(s, seq);	/* unknown */
	out_uint16_le(s, 0x32);	/* entry size */

	s_mark_end(s);
	rdp_send_data(s, RDP_DATA_PDU_FONT2);
}