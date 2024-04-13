static int temac_set_mac_address(struct net_device *ndev, void *p)
{
	struct sockaddr *addr = p;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;
	memcpy(ndev->dev_addr, addr->sa_data, ETH_ALEN);
	temac_do_set_mac_address(ndev);
	return 0;
}