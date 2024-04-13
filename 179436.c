int get_compat_itimerval(struct itimerval *o, const struct compat_itimerval __user *i)
{
	struct compat_itimerval v32;

	if (copy_from_user(&v32, i, sizeof(struct compat_itimerval)))
		return -EFAULT;
	o->it_interval.tv_sec = v32.it_interval.tv_sec;
	o->it_interval.tv_usec = v32.it_interval.tv_usec;
	o->it_value.tv_sec = v32.it_value.tv_sec;
	o->it_value.tv_usec = v32.it_value.tv_usec;
	return 0;
}