static int rtc_ioctl(unsigned fd, unsigned cmd, void __user *argp)
{
	mm_segment_t oldfs = get_fs();
	compat_ulong_t val32;
	unsigned long kval;
	int ret;

	switch (cmd) {
	case RTC_IRQP_READ32:
	case RTC_EPOCH_READ32:
		set_fs(KERNEL_DS);
		ret = sys_ioctl(fd, (cmd == RTC_IRQP_READ32) ?
					RTC_IRQP_READ : RTC_EPOCH_READ,
					(unsigned long)&kval);
		set_fs(oldfs);
		if (ret)
			return ret;
		val32 = kval;
		return put_user(val32, (unsigned int __user *)argp);
	case RTC_IRQP_SET32:
		return sys_ioctl(fd, RTC_IRQP_SET, (unsigned long)argp);
	case RTC_EPOCH_SET32:
		return sys_ioctl(fd, RTC_EPOCH_SET, (unsigned long)argp);
	}

	return -ENOIOCTLCMD;
}