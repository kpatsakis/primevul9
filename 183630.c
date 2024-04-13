static void ____ip_mc_inc_group(struct in_device *in_dev, __be32 addr,
				unsigned int mode, gfp_t gfp)
{
	struct ip_mc_list *im;

	ASSERT_RTNL();

	for_each_pmc_rtnl(in_dev, im) {
		if (im->multiaddr == addr) {
			im->users++;
			ip_mc_add_src(in_dev, &addr, mode, 0, NULL, 0);
			goto out;
		}
	}

	im = kzalloc(sizeof(*im), gfp);
	if (!im)
		goto out;

	im->users = 1;
	im->interface = in_dev;
	in_dev_hold(in_dev);
	im->multiaddr = addr;
	/* initial mode is (EX, empty) */
	im->sfmode = mode;
	im->sfcount[mode] = 1;
	refcount_set(&im->refcnt, 1);
	spin_lock_init(&im->lock);
#ifdef CONFIG_IP_MULTICAST
	timer_setup(&im->timer, igmp_timer_expire, 0);
#endif

	im->next_rcu = in_dev->mc_list;
	in_dev->mc_count++;
	rcu_assign_pointer(in_dev->mc_list, im);

	ip_mc_hash_add(in_dev, im);

#ifdef CONFIG_IP_MULTICAST
	igmpv3_del_delrec(in_dev, im);
#endif
	igmp_group_added(im);
	if (!in_dev->dead)
		ip_rt_multicast_event(in_dev);
out:
	return;
}