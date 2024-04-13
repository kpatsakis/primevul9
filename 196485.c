static int rpmsg_char_init(void)
{
	int ret;

	ret = alloc_chrdev_region(&rpmsg_major, 0, RPMSG_DEV_MAX, "rpmsg");
	if (ret < 0) {
		pr_err("rpmsg: failed to allocate char dev region\n");
		return ret;
	}

	rpmsg_class = class_create(THIS_MODULE, "rpmsg");
	if (IS_ERR(rpmsg_class)) {
		pr_err("failed to create rpmsg class\n");
		unregister_chrdev_region(rpmsg_major, RPMSG_DEV_MAX);
		return PTR_ERR(rpmsg_class);
	}

	ret = register_rpmsg_driver(&rpmsg_chrdev_driver);
	if (ret < 0) {
		pr_err("rpmsgchr: failed to register rpmsg driver\n");
		class_destroy(rpmsg_class);
		unregister_chrdev_region(rpmsg_major, RPMSG_DEV_MAX);
	}

	return ret;
}