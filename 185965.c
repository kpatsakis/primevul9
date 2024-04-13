static int do_i2c_rdwr_ioctl(unsigned int fd, unsigned int cmd,
			struct i2c_rdwr_ioctl_data32    __user *udata)
{
	struct i2c_rdwr_aligned		__user *tdata;
	struct i2c_msg			__user *tmsgs;
	struct i2c_msg32		__user *umsgs;
	compat_caddr_t			datap;
	u32				nmsgs;
	int				i;

	if (get_user(nmsgs, &udata->nmsgs))
		return -EFAULT;
	if (nmsgs > I2C_RDWR_IOCTL_MAX_MSGS)
		return -EINVAL;

	if (get_user(datap, &udata->msgs))
		return -EFAULT;
	umsgs = compat_ptr(datap);

	tdata = compat_alloc_user_space(sizeof(*tdata) +
				      nmsgs * sizeof(struct i2c_msg));
	tmsgs = &tdata->msgs[0];

	if (put_user(nmsgs, &tdata->cmd.nmsgs) ||
	    put_user(tmsgs, &tdata->cmd.msgs))
		return -EFAULT;

	for (i = 0; i < nmsgs; i++) {
		if (copy_in_user(&tmsgs[i].addr, &umsgs[i].addr, 3*sizeof(u16)))
			return -EFAULT;
		if (get_user(datap, &umsgs[i].buf) ||
		    put_user(compat_ptr(datap), &tmsgs[i].buf))
			return -EFAULT;
	}
	return sys_ioctl(fd, cmd, (unsigned long)tdata);
}