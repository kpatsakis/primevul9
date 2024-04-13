isdn_close(struct inode *ino, struct file *filep)
{
	uint minor = iminor(ino);

	mutex_lock(&isdn_mutex);
	if (minor == ISDN_MINOR_STATUS) {
		infostruct *p = dev->infochain;
		infostruct *q = NULL;

		while (p) {
			if (p->private == (char *) &(filep->private_data)) {
				if (q)
					q->next = p->next;
				else
					dev->infochain = (infostruct *) (p->next);
				kfree(p);
				goto out;
			}
			q = p;
			p = (infostruct *) (p->next);
		}
		printk(KERN_WARNING "isdn: No private data while closing isdnctrl\n");
		goto out;
	}
	isdn_unlock_drivers();
	if (minor <= ISDN_MINOR_BMAX)
		goto out;
	if (minor <= ISDN_MINOR_CTRLMAX) {
		if (dev->profd == current)
			dev->profd = NULL;
		goto out;
	}
#ifdef CONFIG_ISDN_PPP
	if (minor <= ISDN_MINOR_PPPMAX)
		isdn_ppp_release(minor - ISDN_MINOR_PPP, filep);
#endif

out:
	mutex_unlock(&isdn_mutex);
	return 0;
}