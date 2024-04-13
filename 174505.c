ber_out_integer(STREAM s, int value)
{
	ber_out_header(s, BER_TAG_INTEGER, 2);
	out_uint16_be(s, value);
}