void ip_mc_up(struct in_device *in_dev)
{
	struct ip_mc_list *pmc;

	ASSERT_RTNL();

	ip_mc_reset(in_dev);
	ip_mc_inc_group(in_dev, IGMP_ALL_HOSTS);

	for_each_pmc_rtnl(in_dev, pmc) {
#ifdef CONFIG_IP_MULTICAST
		igmpv3_del_delrec(in_dev, pmc);
#endif
		igmp_group_added(pmc);
	}
}