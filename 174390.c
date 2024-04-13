ber_out_header(STREAM s, int tagval, int length)
{
	if (tagval > 0xff)
	{
		out_uint16_be(s, tagval);
	}
	else
	{
		out_uint8(s, tagval);
	}

	if (length >= 0x80)
	{
		out_uint8(s, 0x82);
		out_uint16_be(s, length);
	}
	else
		out_uint8(s, length);
}