isdn_net_get_stats(struct net_device *dev)
{
	isdn_net_local *lp = netdev_priv(dev);
	return &lp->stats;
}