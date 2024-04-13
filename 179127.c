off_t size_autodetect(int export) {
	off_t es;
	u32 es32;
	struct stat stat_buf;
	int error;

#ifdef HAVE_SYS_MOUNT_H
#ifdef HAVE_SYS_IOCTL_H
#ifdef BLKGETSIZE
	DEBUG("looking for export size with ioctl BLKGETSIZE\n");
	if (!ioctl(export, BLKGETSIZE, &es32) && es32) {
		es = (off_t)es32 * (off_t)512;
		return es;
	}
#endif /* BLKGETSIZE */
#endif /* HAVE_SYS_IOCTL_H */
#endif /* HAVE_SYS_MOUNT_H */

	DEBUG("looking for export size with fstat\n");
	stat_buf.st_size = 0;
	error = fstat(export, &stat_buf);
	if (!error) {
		if(stat_buf.st_size > 0)
			return (off_t)stat_buf.st_size;
        } else {
                err("fstat failed: %m");
        }

	DEBUG("looking for export size with lseek SEEK_END\n");
	es = lseek(export, (off_t)0, SEEK_END);
	if (es > ((off_t)0)) {
		return es;
        } else {
                DEBUG2("lseek failed: %d", errno==EBADF?1:(errno==ESPIPE?2:(errno==EINVAL?3:4)));
        }

	err("Could not find size of exported block device: %m");
	return OFFT_MAX;
}