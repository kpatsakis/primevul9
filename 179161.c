int __init nfc_genl_init(void)
{
	int rc;

	rc = genl_register_family(&nfc_genl_family);
	if (rc)
		return rc;

	netlink_register_notifier(&nl_notifier);

	return 0;
}