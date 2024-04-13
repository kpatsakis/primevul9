static int bnx2x_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	struct bnx2x *bp = netdev_priv(dev);
	struct mii_ioctl_data *mdio = if_mii(ifr);

	if (!netif_running(dev))
		return -EAGAIN;

	switch (cmd) {
	case SIOCSHWTSTAMP:
		return bnx2x_hwtstamp_ioctl(bp, ifr);
	default:
		DP(NETIF_MSG_LINK, "ioctl: phy id 0x%x, reg 0x%x, val_in 0x%x\n",
		   mdio->phy_id, mdio->reg_num, mdio->val_in);
		return mdio_mii_ioctl(&bp->mdio, mdio, cmd);
	}
}