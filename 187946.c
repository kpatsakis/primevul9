static long fsl_hv_ioctl(struct file *file, unsigned int cmd,
			 unsigned long argaddr)
{
	void __user *arg = (void __user *)argaddr;
	long ret;

	switch (cmd) {
	case FSL_HV_IOCTL_PARTITION_RESTART:
		ret = ioctl_restart(arg);
		break;
	case FSL_HV_IOCTL_PARTITION_GET_STATUS:
		ret = ioctl_status(arg);
		break;
	case FSL_HV_IOCTL_PARTITION_START:
		ret = ioctl_start(arg);
		break;
	case FSL_HV_IOCTL_PARTITION_STOP:
		ret = ioctl_stop(arg);
		break;
	case FSL_HV_IOCTL_MEMCPY:
		ret = ioctl_memcpy(arg);
		break;
	case FSL_HV_IOCTL_DOORBELL:
		ret = ioctl_doorbell(arg);
		break;
	case FSL_HV_IOCTL_GETPROP:
		ret = ioctl_dtprop(arg, 0);
		break;
	case FSL_HV_IOCTL_SETPROP:
		ret = ioctl_dtprop(arg, 1);
		break;
	default:
		pr_debug("fsl-hv: bad ioctl dir=%u type=%u cmd=%u size=%u\n",
			 _IOC_DIR(cmd), _IOC_TYPE(cmd), _IOC_NR(cmd),
			 _IOC_SIZE(cmd));
		return -ENOTTY;
	}

	return ret;
}