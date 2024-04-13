static inline int mmap_is_legacy(void)
{
	if (current->personality & ADDR_COMPAT_LAYOUT)
		return 1;

	if (current->signal->rlim[RLIMIT_STACK].rlim_cur == RLIM_INFINITY)
		return 1;

	return sysctl_legacy_va_layout;
}