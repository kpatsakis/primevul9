word_read_fib(int fd, mso_fib_t *fib)
{
	struct {
		uint32_t offset;
		uint32_t len;
	} macro_details;

	if(!seekandread(fd, 0x118, SEEK_SET, &macro_details, sizeof(macro_details))) {
		cli_dbgmsg("read word_fib failed\n");
		return FALSE;
	}
	fib->macro_offset = vba_endian_convert_32(macro_details.offset, FALSE);
	fib->macro_len = vba_endian_convert_32(macro_details.len, FALSE);

	return TRUE;
}