static void rpmsg_chrdev_exit(void)
{
	unregister_rpmsg_driver(&rpmsg_chrdev_driver);
	class_destroy(rpmsg_class);
	unregister_chrdev_region(rpmsg_major, RPMSG_DEV_MAX);
}