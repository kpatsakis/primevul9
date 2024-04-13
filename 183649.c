static void __igmp_group_dropped(struct ip_mc_list *im, gfp_t gfp)
{
	struct in_device *in_dev = im->interface;
#ifdef CONFIG_IP_MULTICAST
	struct net *net = dev_net(in_dev->dev);
	int reporter;
#endif

	if (im->loaded) {
		im->loaded = 0;
		ip_mc_filter_del(in_dev, im->multiaddr);
	}

#ifdef CONFIG_IP_MULTICAST
	if (im->multiaddr == IGMP_ALL_HOSTS)
		return;
	if (ipv4_is_local_multicast(im->multiaddr) && !net->ipv4.sysctl_igmp_llm_reports)
		return;

	reporter = im->reporter;
	igmp_stop_timer(im);

	if (!in_dev->dead) {
		if (IGMP_V1_SEEN(in_dev))
			return;
		if (IGMP_V2_SEEN(in_dev)) {
			if (reporter)
				igmp_send_report(in_dev, im, IGMP_HOST_LEAVE_MESSAGE);
			return;
		}
		/* IGMPv3 */
		igmpv3_add_delrec(in_dev, im, gfp);

		igmp_ifc_event(in_dev);
	}
#endif
}