int memfd_create(const char *name, unsigned int flags)
{
	return syscall(SYS_memfd_create, name, flags);
}