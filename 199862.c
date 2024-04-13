asmlinkage long compat_sys_futimesat(unsigned int dfd, char __user *filename, struct compat_timeval __user *t)
{
	struct timeval tv[2];

	if (t) {
		if (get_user(tv[0].tv_sec, &t[0].tv_sec) ||
		    get_user(tv[0].tv_usec, &t[0].tv_usec) ||
		    get_user(tv[1].tv_sec, &t[1].tv_sec) ||
		    get_user(tv[1].tv_usec, &t[1].tv_usec))
			return -EFAULT;
	}
	return do_utimes(dfd, filename, t ? tv : NULL);
}