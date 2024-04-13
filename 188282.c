static int temac_init_mac_address(struct net_device *ndev, const void *address)
{
	memcpy(ndev->dev_addr, address, ETH_ALEN);
	if (!is_valid_ether_addr(ndev->dev_addr))
		eth_hw_addr_random(ndev);
	temac_do_set_mac_address(ndev);
	return 0;
}