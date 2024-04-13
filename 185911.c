int ipv6_sysctl_rtcache_flush(struct ctl_table *ctl, int write,
			      void __user *buffer, size_t *lenp, loff_t *ppos)
{
	struct net *net;
	int delay;
	if (!write)
		return -EINVAL;

	net = (struct net *)ctl->extra1;
	delay = net->ipv6.sysctl.flush_delay;
	proc_dointvec(ctl, write, buffer, lenp, ppos);
	fib6_run_gc(delay <= 0 ? 0 : (unsigned long)delay, net, delay > 0);
	return 0;
}