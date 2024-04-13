static void aarp_send_probe_phase1(struct atalk_iface *iface)
{
	struct ifreq atreq;
	struct sockaddr_at *sa = (struct sockaddr_at *)&atreq.ifr_addr;
	const struct net_device_ops *ops = iface->dev->netdev_ops;

	sa->sat_addr.s_node = iface->address.s_node;
	sa->sat_addr.s_net = ntohs(iface->address.s_net);

	/* We pass the Net:Node to the drivers/cards by a Device ioctl. */
	if (!(ops->ndo_do_ioctl(iface->dev, &atreq, SIOCSIFADDR))) {
		ops->ndo_do_ioctl(iface->dev, &atreq, SIOCGIFADDR);
		if (iface->address.s_net != htons(sa->sat_addr.s_net) ||
		    iface->address.s_node != sa->sat_addr.s_node)
			iface->status |= ATIF_PROBE_FAIL;

		iface->address.s_net  = htons(sa->sat_addr.s_net);
		iface->address.s_node = sa->sat_addr.s_node;
	}
}