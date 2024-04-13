cdf_read(const cdf_info_t *info, off_t off, void *buf, size_t len)
{
	size_t siz = CAST(size_t, off + len);

	if (CAST(off_t, off + len) != CAST(off_t, siz))
		goto out;

	if (info->i_buf != NULL && info->i_len >= siz) {
		(void)memcpy(buf, &info->i_buf[off], len);
		return CAST(ssize_t, len);
	}

	if (info->i_fd == -1)
		goto out;

	if (pread(info->i_fd, buf, len, off) != CAST(ssize_t, len))
		return -1;

	return CAST(ssize_t, len);
out:
	errno = EINVAL;
	return -1;
}