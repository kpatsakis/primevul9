static int sp_set_mac_address(struct net_device *dev, void *addr)
{
	struct sockaddr_ax25 *sa = addr;

	netif_tx_lock_bh(dev);
	netif_addr_lock(dev);
	memcpy(dev->dev_addr, &sa->sax25_call, AX25_ADDR_LEN);
	netif_addr_unlock(dev);
	netif_tx_unlock_bh(dev);

	return 0;
}