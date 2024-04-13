isdn_info_update(void)
{
	infostruct *p = dev->infochain;

	while (p) {
		*(p->private) = 1;
		p = (infostruct *) p->next;
	}
	wake_up_interruptible(&(dev->info_waitq));
}