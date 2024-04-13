static int ax88179_mdio_read(struct net_device *netdev, int phy_id, int loc)
{
	struct usbnet *dev = netdev_priv(netdev);
	u16 res;

	ax88179_read_cmd(dev, AX_ACCESS_PHY, phy_id, (__u16)loc, 2, &res);
	return res;
}