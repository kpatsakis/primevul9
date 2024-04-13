static bool igmp_heard_report(struct in_device *in_dev, __be32 group)
{
	struct ip_mc_list *im;
	struct net *net = dev_net(in_dev->dev);

	/* Timers are only set for non-local groups */

	if (group == IGMP_ALL_HOSTS)
		return false;
	if (ipv4_is_local_multicast(group) && !net->ipv4.sysctl_igmp_llm_reports)
		return false;

	rcu_read_lock();
	for_each_pmc_rcu(in_dev, im) {
		if (im->multiaddr == group) {
			igmp_stop_timer(im);
			break;
		}
	}
	rcu_read_unlock();
	return false;
}