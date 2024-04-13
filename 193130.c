COMPAT_SYSCALL_DEFINE1(times, struct compat_tms __user *, tbuf)
{
	if (tbuf) {
		struct tms tms;
		struct compat_tms tmp;

		do_sys_times(&tms);
		/* Convert our struct tms to the compat version. */
		tmp.tms_utime = clock_t_to_compat_clock_t(tms.tms_utime);
		tmp.tms_stime = clock_t_to_compat_clock_t(tms.tms_stime);
		tmp.tms_cutime = clock_t_to_compat_clock_t(tms.tms_cutime);
		tmp.tms_cstime = clock_t_to_compat_clock_t(tms.tms_cstime);
		if (copy_to_user(tbuf, &tmp, sizeof(tmp)))
			return -EFAULT;
	}
	force_successful_syscall_return();
	return compat_jiffies_to_clock_t(jiffies);
}