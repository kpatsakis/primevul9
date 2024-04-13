static int igmpv3_send_report(struct in_device *in_dev, struct ip_mc_list *pmc)
{
	struct sk_buff *skb = NULL;
	struct net *net = dev_net(in_dev->dev);
	int type;

	if (!pmc) {
		rcu_read_lock();
		for_each_pmc_rcu(in_dev, pmc) {
			if (pmc->multiaddr == IGMP_ALL_HOSTS)
				continue;
			if (ipv4_is_local_multicast(pmc->multiaddr) &&
			     !net->ipv4.sysctl_igmp_llm_reports)
				continue;
			spin_lock_bh(&pmc->lock);
			if (pmc->sfcount[MCAST_EXCLUDE])
				type = IGMPV3_MODE_IS_EXCLUDE;
			else
				type = IGMPV3_MODE_IS_INCLUDE;
			skb = add_grec(skb, pmc, type, 0, 0);
			spin_unlock_bh(&pmc->lock);
		}
		rcu_read_unlock();
	} else {
		spin_lock_bh(&pmc->lock);
		if (pmc->sfcount[MCAST_EXCLUDE])
			type = IGMPV3_MODE_IS_EXCLUDE;
		else
			type = IGMPV3_MODE_IS_INCLUDE;
		skb = add_grec(skb, pmc, type, 0, 0);
		spin_unlock_bh(&pmc->lock);
	}
	if (!skb)
		return 0;
	return igmpv3_sendpack(skb);
}