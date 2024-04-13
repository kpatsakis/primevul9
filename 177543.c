isdn_write(struct file *file, const char __user *buf, size_t count, loff_t *off)
{
	uint minor = iminor(file_inode(file));
	int drvidx;
	int chidx;
	int retval;

	if (minor == ISDN_MINOR_STATUS)
		return -EPERM;
	if (!dev->drivers)
		return -ENODEV;

	mutex_lock(&isdn_mutex);
	if (minor <= ISDN_MINOR_BMAX) {
		printk(KERN_WARNING "isdn_write minor %d obsolete!\n", minor);
		drvidx = isdn_minor2drv(minor);
		if (drvidx < 0) {
			retval = -ENODEV;
			goto out;
		}
		if (!(dev->drv[drvidx]->flags & DRV_FLAG_RUNNING)) {
			retval = -ENODEV;
			goto out;
		}
		chidx = isdn_minor2chan(minor);
		wait_event_interruptible(dev->drv[drvidx]->snd_waitq[chidx],
			(retval = isdn_writebuf_stub(drvidx, chidx, buf, count)));
		goto out;
	}
	if (minor <= ISDN_MINOR_CTRLMAX) {
		drvidx = isdn_minor2drv(minor - ISDN_MINOR_CTRL);
		if (drvidx < 0) {
			retval = -ENODEV;
			goto out;
		}
		/*
		 * We want to use the isdnctrl device to load the firmware
		 *
		 if (!(dev->drv[drvidx]->flags & DRV_FLAG_RUNNING))
		 return -ENODEV;
		*/
		if (dev->drv[drvidx]->interface->writecmd)
			retval = dev->drv[drvidx]->interface->
				writecmd(buf, count, drvidx,
					 isdn_minor2chan(minor - ISDN_MINOR_CTRL));
		else
			retval = count;
		goto out;
	}
#ifdef CONFIG_ISDN_PPP
	if (minor <= ISDN_MINOR_PPPMAX) {
		retval = isdn_ppp_write(minor - ISDN_MINOR_PPP, file, buf, count);
		goto out;
	}
#endif
	retval = -ENODEV;
out:
	mutex_unlock(&isdn_mutex);
	return retval;
}