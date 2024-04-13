static inline long get_compat_itimerval(struct itimerval *o,
		struct compat_itimerval __user *i)
{
	return (!access_ok(VERIFY_READ, i, sizeof(*i)) ||
		(__get_user(o->it_interval.tv_sec, &i->it_interval.tv_sec) |
		 __get_user(o->it_interval.tv_usec, &i->it_interval.tv_usec) |
		 __get_user(o->it_value.tv_sec, &i->it_value.tv_sec) |
		 __get_user(o->it_value.tv_usec, &i->it_value.tv_usec)));
}