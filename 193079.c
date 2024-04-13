static int __compat_get_timeval(struct timeval *tv, const struct compat_timeval __user *ctv)
{
	return (!access_ok(VERIFY_READ, ctv, sizeof(*ctv)) ||
			__get_user(tv->tv_sec, &ctv->tv_sec) ||
			__get_user(tv->tv_usec, &ctv->tv_usec)) ? -EFAULT : 0;
}