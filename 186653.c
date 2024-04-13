seekandread(int fd, off_t offset, int whence, void *data, size_t len)
{
	if(lseek(fd, offset, whence) == (off_t)-1) {
		cli_dbgmsg("lseek failed\n");
		return FALSE;
	}
	return cli_readn(fd, data, (unsigned int)len) == (int)len;
}