rdpsnd_init_packet(uint16 type, uint16 size)
{
	STREAM s;

	s = channel_init(rdpsnd_channel, size + 4);
	out_uint16_le(s, type);
	out_uint16_le(s, size);
	return s;
}