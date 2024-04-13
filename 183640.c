void ip_mc_down(struct in_device *in_dev)
{
	struct ip_mc_list *pmc;

	ASSERT_RTNL();

	for_each_pmc_rtnl(in_dev, pmc)
		igmp_group_dropped(pmc);

#ifdef CONFIG_IP_MULTICAST
	in_dev->mr_ifc_count = 0;
	if (del_timer(&in_dev->mr_ifc_timer))
		__in_dev_put(in_dev);
	in_dev->mr_gq_running = 0;
	if (del_timer(&in_dev->mr_gq_timer))
		__in_dev_put(in_dev);
#endif

	ip_mc_dec_group(in_dev, IGMP_ALL_HOSTS);
}