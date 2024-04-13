static inline long put_compat_itimerval(struct compat_itimerval __user *o,
		struct itimerval *i)
{
	return (!access_ok(VERIFY_WRITE, o, sizeof(*o)) ||
		(__put_user(i->it_interval.tv_sec, &o->it_interval.tv_sec) |
		 __put_user(i->it_interval.tv_usec, &o->it_interval.tv_usec) |
		 __put_user(i->it_value.tv_sec, &o->it_value.tv_sec) |
		 __put_user(i->it_value.tv_usec, &o->it_value.tv_usec)));
}