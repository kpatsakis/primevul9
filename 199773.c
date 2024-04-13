int addrconf_sysctl_forward(struct ctl_table *ctl, int write,
			   void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int *valp = ctl->data;
	int val = *valp;
	loff_t pos = *ppos;
	struct ctl_table lctl;
	int ret;

	/*
	 * ctl->data points to idev->cnf.forwarding, we should
	 * not modify it until we get the rtnl lock.
	 */
	lctl = *ctl;
	lctl.data = &val;

	ret = proc_dointvec(&lctl, write, buffer, lenp, ppos);

	if (write)
		ret = addrconf_fixup_forwarding(ctl, valp, val);
	if (ret)
		*ppos = pos;
	return ret;
}