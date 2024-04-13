void ip_mc_init_dev(struct in_device *in_dev)
{
	ASSERT_RTNL();

#ifdef CONFIG_IP_MULTICAST
	timer_setup(&in_dev->mr_gq_timer, igmp_gq_timer_expire, 0);
	timer_setup(&in_dev->mr_ifc_timer, igmp_ifc_timer_expire, 0);
#endif
	ip_mc_reset(in_dev);

	spin_lock_init(&in_dev->mc_tomb_lock);
}