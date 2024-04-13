SYSCALL_DEFINE3(dup3, unsigned int, oldfd, unsigned int, newfd, int, flags)
{
	return ksys_dup3(oldfd, newfd, flags);
}