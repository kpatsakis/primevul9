static int mptctl_do_reset(MPT_ADAPTER *iocp, unsigned long arg)
{
	struct mpt_ioctl_diag_reset __user *urinfo = (void __user *) arg;
	struct mpt_ioctl_diag_reset krinfo;

	if (copy_from_user(&krinfo, urinfo, sizeof(struct mpt_ioctl_diag_reset))) {
		printk(KERN_ERR MYNAM "%s@%d::mptctl_do_reset - "
				"Unable to copy mpt_ioctl_diag_reset struct @ %p\n",
				__FILE__, __LINE__, urinfo);
		return -EFAULT;
	}

	dctlprintk(iocp, printk(MYIOC_s_DEBUG_FMT "mptctl_do_reset called.\n",
	    iocp->name));

	if (mpt_HardResetHandler(iocp, CAN_SLEEP) != 0) {
		printk (MYIOC_s_ERR_FMT "%s@%d::mptctl_do_reset - reset failed.\n",
			iocp->name, __FILE__, __LINE__);
		return -1;
	}

	return 0;
}