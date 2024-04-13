ber_in_header(STREAM s, int *tagval, int *decoded_len)
{
	in_uint8(s, *tagval);
	in_uint8(s, *decoded_len);

	if (*decoded_len < 0x80)
		return True;
	else if (*decoded_len == 0x81)
	{
		in_uint8(s, *decoded_len);
		return True;
	}
	else if (*decoded_len == 0x82)
	{
		in_uint16_be(s, *decoded_len);
		return True;
	}

	return False;
}