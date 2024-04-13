static int do_i2c_smbus_ioctl(unsigned int fd, unsigned int cmd,
			struct i2c_smbus_ioctl_data32   __user *udata)
{
	struct i2c_smbus_ioctl_data	__user *tdata;
	compat_caddr_t			datap;

	tdata = compat_alloc_user_space(sizeof(*tdata));
	if (tdata == NULL)
		return -ENOMEM;
	if (!access_ok(VERIFY_WRITE, tdata, sizeof(*tdata)))
		return -EFAULT;

	if (!access_ok(VERIFY_READ, udata, sizeof(*udata)))
		return -EFAULT;

	if (__copy_in_user(&tdata->read_write, &udata->read_write, 2 * sizeof(u8)))
		return -EFAULT;
	if (__copy_in_user(&tdata->size, &udata->size, 2 * sizeof(u32)))
		return -EFAULT;
	if (__get_user(datap, &udata->data) ||
	    __put_user(compat_ptr(datap), &tdata->data))
		return -EFAULT;

	return sys_ioctl(fd, cmd, (unsigned long)tdata);
}