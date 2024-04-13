mptctl_mpt_command (MPT_ADAPTER *ioc, unsigned long arg)
{
	struct mpt_ioctl_command __user *uarg = (void __user *) arg;
	struct mpt_ioctl_command  karg;
	int		rc;


	if (copy_from_user(&karg, uarg, sizeof(struct mpt_ioctl_command))) {
		printk(KERN_ERR MYNAM "%s@%d::mptctl_mpt_command - "
			"Unable to read in mpt_ioctl_command struct @ %p\n",
				__FILE__, __LINE__, uarg);
		return -EFAULT;
	}

	rc = mptctl_do_mpt_command (ioc, karg, &uarg->MF);

	return rc;
}