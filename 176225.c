static int ax88179_set_mac_addr(struct net_device *net, void *p)
{
	struct usbnet *dev = netdev_priv(net);
	struct sockaddr *addr = p;
	int ret;

	if (netif_running(net))
		return -EBUSY;
	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	eth_hw_addr_set(net, addr->sa_data);

	/* Set the MAC address */
	ret = ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_NODE_ID, ETH_ALEN,
				 ETH_ALEN, net->dev_addr);
	if (ret < 0)
		return ret;

	return 0;
}