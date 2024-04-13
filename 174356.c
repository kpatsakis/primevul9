rdp_out_unknown_caps(STREAM s, uint16 id, uint16 length, uint8 * caps)
{
	out_uint16_le(s, id);
	out_uint16_le(s, length);

	out_uint8p(s, caps, length - 4);
}