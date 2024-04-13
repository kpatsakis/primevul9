isdn_poll(struct file *file, poll_table *wait)
{
	unsigned int mask = 0;
	unsigned int minor = iminor(file_inode(file));
	int drvidx = isdn_minor2drv(minor - ISDN_MINOR_CTRL);

	mutex_lock(&isdn_mutex);
	if (minor == ISDN_MINOR_STATUS) {
		poll_wait(file, &(dev->info_waitq), wait);
		/* mask = POLLOUT | POLLWRNORM; */
		if (file->private_data) {
			mask |= POLLIN | POLLRDNORM;
		}
		goto out;
	}
	if (minor >= ISDN_MINOR_CTRL && minor <= ISDN_MINOR_CTRLMAX) {
		if (drvidx < 0) {
			/* driver deregistered while file open */
			mask = POLLHUP;
			goto out;
		}
		poll_wait(file, &(dev->drv[drvidx]->st_waitq), wait);
		mask = POLLOUT | POLLWRNORM;
		if (dev->drv[drvidx]->stavail) {
			mask |= POLLIN | POLLRDNORM;
		}
		goto out;
	}
#ifdef CONFIG_ISDN_PPP
	if (minor <= ISDN_MINOR_PPPMAX) {
		mask = isdn_ppp_poll(file, wait);
		goto out;
	}
#endif
	mask = POLLERR;
out:
	mutex_unlock(&isdn_mutex);
	return mask;
}