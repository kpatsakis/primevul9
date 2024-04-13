unsigned long __fdget(unsigned int fd)
{
	return __fget_light(fd, FMODE_PATH);
}