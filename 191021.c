static void smb_dump(const char *name, int type, const char *data, ssize_t len)
{
	int fd, i;
	char *fname = NULL;
	if (DEBUGLEVEL < 50) {
		return;
	}

	if (len < 4) len = smb_len(data)+4;
	for (i=1;i<100;i++) {
		if (asprintf(&fname, "/tmp/%s.%d.%s", name, i,
			     type ? "req" : "resp") == -1) {
			return;
		}
		fd = open(fname, O_WRONLY|O_CREAT|O_EXCL, 0644);
		if (fd != -1 || errno != EEXIST) break;
	}
	if (fd != -1) {
		ssize_t ret = write(fd, data, len);
		if (ret != len)
			DEBUG(0,("smb_dump: problem: write returned %d\n", (int)ret ));
		close(fd);
		DEBUG(0,("created %s len %lu\n", fname, (unsigned long)len));
	}
	SAFE_FREE(fname);
}