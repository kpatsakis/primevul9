word_skip_oxo3(int fd)
{
	uint8_t count;

	if (cli_readn(fd, &count, 1) != 1) {
		cli_dbgmsg("read oxo3 record1 failed\n");
		return FALSE;
	}
	cli_dbgmsg("oxo3 records1: %d\n", count);

	if(!seekandread(fd, count * 14, SEEK_CUR, &count, 1)) {
		cli_dbgmsg("read oxo3 record2 failed\n");
		return FALSE;
	}

	if(count == 0) {
		uint8_t twobytes[2];

		if(cli_readn(fd, twobytes, 2) != 2) {
			cli_dbgmsg("read oxo3 failed\n");
			return FALSE;
		}
		if(twobytes[0] != 2) {
			lseek(fd, -2, SEEK_CUR);
			return TRUE;
		}
		count = twobytes[1];
	}
	if(count > 0)
		if (lseek(fd, (count*4)+1, SEEK_CUR) == -1) {
			cli_dbgmsg("lseek oxo3 failed\n");
			return FALSE;
		}

	cli_dbgmsg("oxo3 records2: %d\n", count);
	return TRUE;
}