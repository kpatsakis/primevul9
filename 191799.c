int netlink_remove_tap(struct netlink_tap *nt)
{
	int ret;

	ret = __netlink_remove_tap(nt);
	synchronize_net();

	return ret;
}