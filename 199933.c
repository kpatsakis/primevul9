static ssize_t bad_file_aio_read(struct kiocb *iocb, const struct iovec *iov,
			unsigned long nr_segs, loff_t pos)
{
	return -EIO;
}