mptctl_syscall_down(MPT_ADAPTER *ioc, int nonblock)
{
	int rc = 0;

	if (nonblock) {
		if (!mutex_trylock(&ioc->ioctl_cmds.mutex))
			rc = -EAGAIN;
	} else {
		if (mutex_lock_interruptible(&ioc->ioctl_cmds.mutex))
			rc = -ERESTARTSYS;
	}
	return rc;
}