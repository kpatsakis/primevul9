static int bad_file_aio_fsync(struct kiocb *iocb, int datasync)
{
	return -EIO;
}