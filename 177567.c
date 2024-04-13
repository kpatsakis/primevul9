static __inline__ int isdn_net_device_started(isdn_net_dev *n)
{
	isdn_net_local *lp = n->local;
	struct net_device *dev;

	if (lp->master)
		dev = lp->master;
	else
		dev = n->dev;
	return netif_running(dev);
}