skip_past_nul(int fd)
{
    char *end;
    char smallbuf[128];

    do {
	int nread = cli_readn(fd, smallbuf, sizeof(smallbuf));
	if (nread <= 0)
	    return FALSE;
	end = memchr(smallbuf, '\0', nread);
	if (end) {
	    if (lseek(fd, 1 + (end-smallbuf) - nread, SEEK_CUR) < 0)
		return FALSE;
	    return TRUE;
	}
    } while (1);
}