static int __compat_put_timeval(const struct timeval *tv, struct compat_timeval __user *ctv)
{
	return (!access_ok(VERIFY_WRITE, ctv, sizeof(*ctv)) ||
			__put_user(tv->tv_sec, &ctv->tv_sec) ||
			__put_user(tv->tv_usec, &ctv->tv_usec)) ? -EFAULT : 0;
}