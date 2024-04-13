mptctl_fw_download(MPT_ADAPTER *iocp, unsigned long arg)
{
	struct mpt_fw_xfer __user *ufwdl = (void __user *) arg;
	struct mpt_fw_xfer	 kfwdl;

	if (copy_from_user(&kfwdl, ufwdl, sizeof(struct mpt_fw_xfer))) {
		printk(KERN_ERR MYNAM "%s@%d::_ioctl_fwdl - "
				"Unable to copy mpt_fw_xfer struct @ %p\n",
				__FILE__, __LINE__, ufwdl);
		return -EFAULT;
	}

	return mptctl_do_fw_download(iocp, kfwdl.bufp, kfwdl.fwlen);
}