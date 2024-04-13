int put_compat_rusage(const struct rusage *r, struct compat_rusage __user *ru)
{
	struct compat_rusage r32;
	memset(&r32, 0, sizeof(r32));
	r32.ru_utime.tv_sec = r->ru_utime.tv_sec;
	r32.ru_utime.tv_usec = r->ru_utime.tv_usec;
	r32.ru_stime.tv_sec = r->ru_stime.tv_sec;
	r32.ru_stime.tv_usec = r->ru_stime.tv_usec;
	r32.ru_maxrss = r->ru_maxrss;
	r32.ru_ixrss = r->ru_ixrss;
	r32.ru_idrss = r->ru_idrss;
	r32.ru_isrss = r->ru_isrss;
	r32.ru_minflt = r->ru_minflt;
	r32.ru_majflt = r->ru_majflt;
	r32.ru_nswap = r->ru_nswap;
	r32.ru_inblock = r->ru_inblock;
	r32.ru_oublock = r->ru_oublock;
	r32.ru_msgsnd = r->ru_msgsnd;
	r32.ru_msgrcv = r->ru_msgrcv;
	r32.ru_nsignals = r->ru_nsignals;
	r32.ru_nvcsw = r->ru_nvcsw;
	r32.ru_nivcsw = r->ru_nivcsw;
	if (copy_to_user(ru, &r32, sizeof(r32)))
		return -EFAULT;
	return 0;
}