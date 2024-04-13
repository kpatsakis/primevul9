isdn_net_bind_channel(isdn_net_local *lp, int idx)
{
	lp->flags |= ISDN_NET_CONNECTED;
	lp->isdn_device = dev->drvmap[idx];
	lp->isdn_channel = dev->chanmap[idx];
	dev->rx_netdev[idx] = lp->netdev;
	dev->st_netdev[idx] = lp->netdev;
}