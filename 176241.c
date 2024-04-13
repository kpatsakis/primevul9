static int ax88179_get_link_ksettings(struct net_device *net,
				      struct ethtool_link_ksettings *cmd)
{
	struct usbnet *dev = netdev_priv(net);

	mii_ethtool_get_link_ksettings(&dev->mii, cmd);

	return 0;
}