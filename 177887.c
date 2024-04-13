misdn_sock_init(u_int *deb)
{
	int err;

	debug = deb;
	err = sock_register(&mISDN_sock_family_ops);
	if (err)
		printk(KERN_ERR "%s: error(%d)\n", __func__, err);
	return err;
}