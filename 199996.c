isdn_open(struct inode *ino, struct file *filep)
{
	uint minor = iminor(ino);
	int drvidx;
	int chidx;
	int retval = -ENODEV;


	if (minor == ISDN_MINOR_STATUS) {
		infostruct *p;

		if ((p = kmalloc(sizeof(infostruct), GFP_KERNEL))) {
			p->next = (char *) dev->infochain;
			p->private = (char *) &(filep->private_data);
			dev->infochain = p;
			/* At opening we allow a single update */
			filep->private_data = (char *) 1;
			retval = 0;
			goto out;
		} else {
			retval = -ENOMEM;
			goto out;
		}
	}
	if (!dev->channels)
		goto out;
	if (minor <= ISDN_MINOR_BMAX) {
		printk(KERN_WARNING "isdn_open minor %d obsolete!\n", minor);
		drvidx = isdn_minor2drv(minor);
		if (drvidx < 0)
			goto out;
		chidx = isdn_minor2chan(minor);
		if (!(dev->drv[drvidx]->flags & DRV_FLAG_RUNNING))
			goto out;
		if (!(dev->drv[drvidx]->online & (1 << chidx)))
			goto out;
		isdn_lock_drivers();
		retval = 0;
		goto out;
	}
	if (minor <= ISDN_MINOR_CTRLMAX) {
		drvidx = isdn_minor2drv(minor - ISDN_MINOR_CTRL);
		if (drvidx < 0)
			goto out;
		isdn_lock_drivers();
		retval = 0;
		goto out;
	}
#ifdef CONFIG_ISDN_PPP
	if (minor <= ISDN_MINOR_PPPMAX) {
		retval = isdn_ppp_open(minor - ISDN_MINOR_PPP, filep);
		if (retval == 0)
			isdn_lock_drivers();
		goto out;
	}
#endif
 out:
	nonseekable_open(ino, filep);
	return retval;
}