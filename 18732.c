static int xemaclite_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	if (!dev->phydev || !netif_running(dev))
		return -EINVAL;

	switch (cmd) {
	case SIOCGMIIPHY:
	case SIOCGMIIREG:
	case SIOCSMIIREG:
		return phy_mii_ioctl(dev->phydev, rq, cmd);
	default:
		return -EOPNOTSUPP;
	}
}