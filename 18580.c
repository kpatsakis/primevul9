unsigned long __fdget_raw(unsigned int fd)
{
	return __fget_light(fd, 0);
}