static int ioctl_fionbio(struct file *filp, int __user *argp)
{
	unsigned int flag;
	int on, error;

	error = get_user(on, argp);
	if (error)
		return error;
	flag = O_NONBLOCK;
#ifdef __sparc__
	/* SunOS compatibility item. */
	if (O_NONBLOCK != O_NDELAY)
		flag |= O_NDELAY;
#endif
	spin_lock(&filp->f_lock);
	if (on)
		filp->f_flags |= flag;
	else
		filp->f_flags &= ~flag;
	spin_unlock(&filp->f_lock);
	return error;
}