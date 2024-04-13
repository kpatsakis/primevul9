static int __init mptctl_init(void)
{
	int err;
	int where = 1;

	show_mptmod_ver(my_NAME, my_VERSION);

	mpt_device_driver_register(&mptctl_driver, MPTCTL_DRIVER);

	/* Register this device */
	err = misc_register(&mptctl_miscdev);
	if (err < 0) {
		printk(KERN_ERR MYNAM ": Can't register misc device [minor=%d].\n", MPT_MINOR);
		goto out_fail;
	}
	printk(KERN_INFO MYNAM ": Registered with Fusion MPT base driver\n");
	printk(KERN_INFO MYNAM ": /dev/%s @ (major,minor=%d,%d)\n",
			 mptctl_miscdev.name, MISC_MAJOR, mptctl_miscdev.minor);

	/*
	 *  Install our handler
	 */
	++where;
	mptctl_id = mpt_register(mptctl_reply, MPTCTL_DRIVER,
	    "mptctl_reply");
	if (!mptctl_id || mptctl_id >= MPT_MAX_PROTOCOL_DRIVERS) {
		printk(KERN_ERR MYNAM ": ERROR: Failed to register with Fusion MPT base driver\n");
		misc_deregister(&mptctl_miscdev);
		err = -EBUSY;
		goto out_fail;
	}

	mptctl_taskmgmt_id = mpt_register(mptctl_taskmgmt_reply, MPTCTL_DRIVER,
	    "mptctl_taskmgmt_reply");
	if (!mptctl_taskmgmt_id || mptctl_taskmgmt_id >= MPT_MAX_PROTOCOL_DRIVERS) {
		printk(KERN_ERR MYNAM ": ERROR: Failed to register with Fusion MPT base driver\n");
		mpt_deregister(mptctl_id);
		misc_deregister(&mptctl_miscdev);
		err = -EBUSY;
		goto out_fail;
	}

	mpt_reset_register(mptctl_id, mptctl_ioc_reset);
	mpt_event_register(mptctl_id, mptctl_event_process);

	return 0;

out_fail:

	mpt_device_driver_deregister(MPTCTL_DRIVER);

	return err;
}