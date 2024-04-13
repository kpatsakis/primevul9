mptctl_ioc_reset(MPT_ADAPTER *ioc, int reset_phase)
{
	switch(reset_phase) {
	case MPT_IOC_SETUP_RESET:
		dtmprintk(ioc, printk(MYIOC_s_DEBUG_FMT
		    "%s: MPT_IOC_SETUP_RESET\n", ioc->name, __func__));
		break;
	case MPT_IOC_PRE_RESET:
		dtmprintk(ioc, printk(MYIOC_s_DEBUG_FMT
		    "%s: MPT_IOC_PRE_RESET\n", ioc->name, __func__));
		break;
	case MPT_IOC_POST_RESET:
		dtmprintk(ioc, printk(MYIOC_s_DEBUG_FMT
		    "%s: MPT_IOC_POST_RESET\n", ioc->name, __func__));
		if (ioc->ioctl_cmds.status & MPT_MGMT_STATUS_PENDING) {
			ioc->ioctl_cmds.status |= MPT_MGMT_STATUS_DID_IOCRESET;
			complete(&ioc->ioctl_cmds.done);
		}
		break;
	default:
		break;
	}

	return 1;
}