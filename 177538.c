static void __exit isdn_exit(void)
{
#ifdef CONFIG_ISDN_PPP
	isdn_ppp_cleanup();
#endif
	if (isdn_net_rmall() < 0) {
		printk(KERN_WARNING "isdn: net-device busy, remove cancelled\n");
		return;
	}
	isdn_tty_exit();
	unregister_chrdev(ISDN_MAJOR, "isdn");
	del_timer_sync(&dev->timer);
	/* call vfree with interrupts enabled, else it will hang */
	vfree(dev);
	printk(KERN_NOTICE "ISDN-subsystem unloaded\n");
}