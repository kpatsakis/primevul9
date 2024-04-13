word_skip_macro_intnames(int fd)
{
	uint16_t count;

	if(!read_uint16(fd, &count, FALSE)) {
		cli_dbgmsg("read macro_intnames failed\n");
		return FALSE;
	}
	cli_dbgmsg("intnames count: %u\n", (unsigned int)count);

	while(count-- > 0) {
		uint8_t length;

		/* id */
		if(!seekandread(fd, sizeof(uint16_t), SEEK_CUR, &length, sizeof(uint8_t))) {
			cli_dbgmsg("skip_macro_intnames failed\n");
			return FALSE;
		}

		/* Internal name, plus one byte of unknown data */
		if(lseek(fd, length + 1, SEEK_CUR) == -1) {
			cli_dbgmsg("skip_macro_intnames failed\n");
			return FALSE;
		}
	}
	return TRUE;
}