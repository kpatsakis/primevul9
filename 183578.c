static void ip_mc_filter_add(struct in_device *in_dev, __be32 addr)
{
	char buf[MAX_ADDR_LEN];
	struct net_device *dev = in_dev->dev;

	/* Checking for IFF_MULTICAST here is WRONG-WRONG-WRONG.
	   We will get multicast token leakage, when IFF_MULTICAST
	   is changed. This check should be done in ndo_set_rx_mode
	   routine. Something sort of:
	   if (dev->mc_list && dev->flags&IFF_MULTICAST) { do it; }
	   --ANK
	   */
	if (arp_mc_map(addr, buf, dev, 0) == 0)
		dev_mc_add(dev, buf);
}