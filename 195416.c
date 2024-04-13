static long compat_mpctl_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	long ret;
	mutex_lock(&mpctl_mutex);
	switch (cmd) {
	case MPTIOCINFO:
	case MPTIOCINFO1:
	case MPTIOCINFO2:
	case MPTTARGETINFO:
	case MPTEVENTQUERY:
	case MPTEVENTENABLE:
	case MPTEVENTREPORT:
	case MPTHARDRESET:
	case HP_GETHOSTINFO:
	case HP_GETTARGETINFO:
	case MPTTEST:
		ret = __mptctl_ioctl(f, cmd, arg);
		break;
	case MPTCOMMAND32:
		ret = compat_mpt_command(f, cmd, arg);
		break;
	case MPTFWDOWNLOAD32:
		ret = compat_mptfwxfer_ioctl(f, cmd, arg);
		break;
	default:
		ret = -ENOIOCTLCMD;
		break;
	}
	mutex_unlock(&mpctl_mutex);
	return ret;
}