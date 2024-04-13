static int isdn_net_ioctl(struct net_device *dev,
			  struct ifreq *ifr, int cmd)
{
	isdn_net_local *lp = netdev_priv(dev);

	switch (lp->p_encap) {
#ifdef CONFIG_ISDN_PPP
	case ISDN_NET_ENCAP_SYNCPPP:
		return isdn_ppp_dev_ioctl(dev, ifr, cmd);
#endif
	case ISDN_NET_ENCAP_CISCOHDLCK:
		return isdn_ciscohdlck_dev_ioctl(dev, ifr, cmd);
	default:
		return -EINVAL;
	}
}