static void igmp_group_added(struct ip_mc_list *im)
{
	struct in_device *in_dev = im->interface;
#ifdef CONFIG_IP_MULTICAST
	struct net *net = dev_net(in_dev->dev);
#endif

	if (im->loaded == 0) {
		im->loaded = 1;
		ip_mc_filter_add(in_dev, im->multiaddr);
	}

#ifdef CONFIG_IP_MULTICAST
	if (im->multiaddr == IGMP_ALL_HOSTS)
		return;
	if (ipv4_is_local_multicast(im->multiaddr) && !net->ipv4.sysctl_igmp_llm_reports)
		return;

	if (in_dev->dead)
		return;

	im->unsolicit_count = net->ipv4.sysctl_igmp_qrv;
	if (IGMP_V1_SEEN(in_dev) || IGMP_V2_SEEN(in_dev)) {
		spin_lock_bh(&im->lock);
		igmp_start_timer(im, IGMP_INITIAL_REPORT_DELAY);
		spin_unlock_bh(&im->lock);
		return;
	}
	/* else, v3 */

	/* Based on RFC3376 5.1, for newly added INCLUDE SSM, we should
	 * not send filter-mode change record as the mode should be from
	 * IN() to IN(A).
	 */
	if (im->sfmode == MCAST_EXCLUDE)
		im->crcount = in_dev->mr_qrv ?: net->ipv4.sysctl_igmp_qrv;

	igmp_ifc_event(in_dev);
#endif
}