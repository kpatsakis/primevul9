static int addrconf_ifid_sit(u8 *eui, struct net_device *dev)
{
	if (dev->priv_flags & IFF_ISATAP)
		return __ipv6_isatap_ifid(eui, *(__be32 *)dev->dev_addr);
	return -1;
}