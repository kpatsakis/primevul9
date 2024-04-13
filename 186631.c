word_skip_menu_info(int fd)
{
	uint16_t count;

	if(!read_uint16(fd, &count, FALSE)) {
		cli_dbgmsg("read menu_info failed\n");
		return FALSE;
	}
	cli_dbgmsg("menu_info count: %d\n", count);

	if(count)
		if(lseek(fd, count * 12, SEEK_CUR) == -1)
			return FALSE;
	return TRUE;
}