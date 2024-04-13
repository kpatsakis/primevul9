read_uint32(int fd, uint32_t *u, int big_endian)
{
	if(cli_readn(fd, u, sizeof(uint32_t)) != sizeof(uint32_t))
		return FALSE;

	*u = vba_endian_convert_32(*u, big_endian);

	return TRUE;
}