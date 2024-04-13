static void addrconf_mod_rs_timer(struct inet6_dev *idev,
				  unsigned long when)
{
	if (!timer_pending(&idev->rs_timer))
		in6_dev_hold(idev);
	mod_timer(&idev->rs_timer, jiffies + when);
}