read_uint16(int fd, uint16_t *u, int big_endian)
{
	if(cli_readn(fd, u, sizeof(uint16_t)) != sizeof(uint16_t))
		return FALSE;

	*u = vba_endian_convert_16(*u, big_endian);

	return TRUE;
}