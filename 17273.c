SYSCALL_DEFINE3(lchown, const char __user *, filename, uid_t, user, gid_t, group)
{
	return sys_fchownat(AT_FDCWD, filename, user, group,
			    AT_SYMLINK_NOFOLLOW);
}