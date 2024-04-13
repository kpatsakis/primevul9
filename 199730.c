static int addrconf_ifid_gre(u8 *eui, struct net_device *dev)
{
	return __ipv6_isatap_ifid(eui, *(__be32 *)dev->dev_addr);
}