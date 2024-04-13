static void addrconf_del_rs_timer(struct inet6_dev *idev)
{
	if (del_timer(&idev->rs_timer))
		__in6_dev_put(idev);
}