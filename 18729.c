static int xemaclite_set_mac_address(struct net_device *dev, void *address)
{
	struct net_local *lp = netdev_priv(dev);
	struct sockaddr *addr = address;

	if (netif_running(dev))
		return -EBUSY;

	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);
	xemaclite_update_address(lp, dev->dev_addr);
	return 0;
}