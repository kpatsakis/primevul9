static int addrconf_ifid_eui64(u8 *eui, struct net_device *dev)
{
	if (dev->addr_len != IEEE802154_ADDR_LEN)
		return -1;
	memcpy(eui, dev->dev_addr, 8);
	eui[0] ^= 2;
	return 0;
}