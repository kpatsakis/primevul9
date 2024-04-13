static void packet_dev_mclist(struct net_device *dev, struct packet_mclist *i, int what)
{
	for ( ; i; i = i->next) {
		if (i->ifindex == dev->ifindex)
			packet_dev_mc(dev, i, what);
	}
}