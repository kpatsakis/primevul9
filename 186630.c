word_skip_macro_extnames(int fd)
{
	int is_unicode, nbytes;
	int16_t size;

	if(!read_uint16(fd, (uint16_t *)&size, FALSE)) {
		cli_dbgmsg("read macro_extnames failed\n");
		return FALSE;
	}
	if (size == -1) { /* Unicode flag */
		if(!read_uint16(fd, (uint16_t *)&size, FALSE)) {
			cli_dbgmsg("read macro_extnames failed\n");
			return FALSE;
		}
		is_unicode = 1;
	} else
		is_unicode = 0;

	cli_dbgmsg("ext names size: 0x%x\n", size);

	nbytes = size;
	while(nbytes > 0) {
		uint8_t length;
		off_t offset;

		if (cli_readn(fd, &length, 1) != 1) {
			cli_dbgmsg("read macro_extnames failed\n");
			return FALSE;
		}

		if(is_unicode)
			offset = (off_t)length * 2 + 1;
		else
			offset = (off_t)length;

		/* ignore numref as well */
		if(lseek(fd, offset + sizeof(uint16_t), SEEK_CUR) == -1) {
			cli_dbgmsg("read macro_extnames failed to seek\n");
			return FALSE;
		}
		nbytes -= size;
	}
	return TRUE;
}