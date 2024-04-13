static void mptctl_exit(void)
{
	misc_deregister(&mptctl_miscdev);
	printk(KERN_INFO MYNAM ": Deregistered /dev/%s @ (major,minor=%d,%d)\n",
			 mptctl_miscdev.name, MISC_MAJOR, mptctl_miscdev.minor);

	/* De-register event handler from base module */
	mpt_event_deregister(mptctl_id);

	/* De-register reset handler from base module */
	mpt_reset_deregister(mptctl_id);

	/* De-register callback handler from base module */
	mpt_deregister(mptctl_taskmgmt_id);
	mpt_deregister(mptctl_id);

        mpt_device_driver_deregister(MPTCTL_DRIVER);

}