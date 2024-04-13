static int mt_ioctl_trans(unsigned int fd, unsigned int cmd, void __user *argp)
{
	mm_segment_t old_fs = get_fs();
	struct mtget get;
	struct mtget32 __user *umget32;
	struct mtpos pos;
	struct mtpos32 __user *upos32;
	unsigned long kcmd;
	void *karg;
	int err = 0;

	switch(cmd) {
	case MTIOCPOS32:
		kcmd = MTIOCPOS;
		karg = &pos;
		break;
	default:	/* MTIOCGET32 */
		kcmd = MTIOCGET;
		karg = &get;
		break;
	}
	set_fs (KERNEL_DS);
	err = sys_ioctl (fd, kcmd, (unsigned long)karg);
	set_fs (old_fs);
	if (err)
		return err;
	switch (cmd) {
	case MTIOCPOS32:
		upos32 = argp;
		err = __put_user(pos.mt_blkno, &upos32->mt_blkno);
		break;
	case MTIOCGET32:
		umget32 = argp;
		err = __put_user(get.mt_type, &umget32->mt_type);
		err |= __put_user(get.mt_resid, &umget32->mt_resid);
		err |= __put_user(get.mt_dsreg, &umget32->mt_dsreg);
		err |= __put_user(get.mt_gstat, &umget32->mt_gstat);
		err |= __put_user(get.mt_erreg, &umget32->mt_erreg);
		err |= __put_user(get.mt_fileno, &umget32->mt_fileno);
		err |= __put_user(get.mt_blkno, &umget32->mt_blkno);
		break;
	}
	return err ? -EFAULT: 0;
}