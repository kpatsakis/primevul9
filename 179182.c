static int nfc_genl_dump_ses_done(struct netlink_callback *cb)
{
	struct class_dev_iter *iter = (struct class_dev_iter *) cb->args[0];

	if (iter) {
		nfc_device_iter_exit(iter);
		kfree(iter);
	}

	return 0;
}