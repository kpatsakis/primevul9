static __inline__ void isdn_net_device_stop_queue(isdn_net_local *lp)
{
	if (lp->master)
		netif_stop_queue(lp->master);
	else
		netif_stop_queue(lp->netdev->dev);
}