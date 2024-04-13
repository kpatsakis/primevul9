static void __exit cpia2_exit(void)
{
	cpia2_usb_cleanup();
	schedule_timeout(2 * HZ);
}