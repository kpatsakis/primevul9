int put_compat_itimerval(struct compat_itimerval __user *o, const struct itimerval *i)
{
	struct compat_itimerval v32;

	v32.it_interval.tv_sec = i->it_interval.tv_sec;
	v32.it_interval.tv_usec = i->it_interval.tv_usec;
	v32.it_value.tv_sec = i->it_value.tv_sec;
	v32.it_value.tv_usec = i->it_value.tv_usec;
	return copy_to_user(o, &v32, sizeof(struct compat_itimerval)) ? -EFAULT : 0;
}