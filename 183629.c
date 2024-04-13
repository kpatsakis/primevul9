void ip_mc_unmap(struct in_device *in_dev)
{
	struct ip_mc_list *pmc;

	ASSERT_RTNL();

	for_each_pmc_rtnl(in_dev, pmc)
		igmp_group_dropped(pmc);
}