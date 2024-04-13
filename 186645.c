vba_endian_convert_32(uint32_t value, int big_endian)
{
	if (big_endian)
		return be32_to_host(value);
	else
		return le32_to_host(value);
}