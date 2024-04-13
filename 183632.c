void ip_mc_remap(struct in_device *in_dev)
{
	struct ip_mc_list *pmc;

	ASSERT_RTNL();

	for_each_pmc_rtnl(in_dev, pmc) {
#ifdef CONFIG_IP_MULTICAST
		igmpv3_del_delrec(in_dev, pmc);
#endif
		igmp_group_added(pmc);
	}
}