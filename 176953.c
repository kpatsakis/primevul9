static void __exit qrtr_tun_exit(void)
{
	misc_deregister(&qrtr_tun_miscdev);
}