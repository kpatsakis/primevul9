struct file *fget_many(unsigned int fd, unsigned int refs)
{
	return __fget(fd, FMODE_PATH, refs);
}