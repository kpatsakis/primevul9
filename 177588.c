static __inline__ void isdn_net_device_wake_queue(isdn_net_local *lp)
{
	if (lp->master)
		netif_wake_queue(lp->master);
	else
		netif_wake_queue(lp->netdev->dev);
}