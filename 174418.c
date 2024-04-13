mcs_send_to_channel(STREAM s, uint16 channel)
{
	uint16 length;

	s_pop_layer(s, mcs_hdr);
	length = s->end - s->p - 8;
	length |= 0x8000;

	out_uint8(s, (MCS_SDRQ << 2));
	out_uint16_be(s, g_mcs_userid);
	out_uint16_be(s, channel);
	out_uint8(s, 0x70);	/* flags */
	out_uint16_be(s, length);

	iso_send(s);
}