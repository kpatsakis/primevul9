vba_endian_convert_16(uint16_t value, int big_endian)
{
	if (big_endian)
		return (uint16_t)be16_to_host(value);
	else
		return le16_to_host(value);
}