mptctl_eventreport (MPT_ADAPTER *ioc, unsigned long arg)
{
	struct mpt_ioctl_eventreport __user *uarg = (void __user *) arg;
	struct mpt_ioctl_eventreport	 karg;
	int			 numBytes, maxEvents, max;

	if (copy_from_user(&karg, uarg, sizeof(struct mpt_ioctl_eventreport))) {
		printk(KERN_ERR MYNAM "%s@%d::mptctl_eventreport - "
			"Unable to read in mpt_ioctl_eventreport struct @ %p\n",
				__FILE__, __LINE__, uarg);
		return -EFAULT;
	}

	dctlprintk(ioc, printk(MYIOC_s_DEBUG_FMT "mptctl_eventreport called.\n",
	    ioc->name));

	numBytes = karg.hdr.maxDataSize - sizeof(mpt_ioctl_header);
	maxEvents = numBytes/sizeof(MPT_IOCTL_EVENTS);


	max = MPTCTL_EVENT_LOG_SIZE < maxEvents ? MPTCTL_EVENT_LOG_SIZE : maxEvents;

	/* If fewer than 1 event is requested, there must have
	 * been some type of error.
	 */
	if ((max < 1) || !ioc->events)
		return -ENODATA;

	/* reset this flag so SIGIO can restart */
	ioc->aen_event_read_flag=0;

	/* Copy the data from kernel memory to user memory
	 */
	numBytes = max * sizeof(MPT_IOCTL_EVENTS);
	if (copy_to_user(uarg->eventData, ioc->events, numBytes)) {
		printk(MYIOC_s_ERR_FMT "%s@%d::mptctl_eventreport - "
			"Unable to write out mpt_ioctl_eventreport struct @ %p\n",
			ioc->name, __FILE__, __LINE__, ioc->events);
		return -EFAULT;
	}

	return 0;
}