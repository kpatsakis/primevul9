isdn_net_unbind_channel(isdn_net_local *lp)
{
	skb_queue_purge(&lp->super_tx_queue);

	if (!lp->master) {	/* reset only master device */
		/* Moral equivalent of dev_purge_queues():
		   BEWARE! This chunk of code cannot be called from hardware
		   interrupt handler. I hope it is true. --ANK
		*/
		qdisc_reset_all_tx(lp->netdev->dev);
	}
	lp->dialstate = 0;
	dev->rx_netdev[isdn_dc2minor(lp->isdn_device, lp->isdn_channel)] = NULL;
	dev->st_netdev[isdn_dc2minor(lp->isdn_device, lp->isdn_channel)] = NULL;
	if (lp->isdn_device != -1 && lp->isdn_channel != -1)
		isdn_free_channel(lp->isdn_device, lp->isdn_channel,
				  ISDN_USAGE_NET);
	lp->flags &= ~ISDN_NET_CONNECTED;
	lp->isdn_device = -1;
	lp->isdn_channel = -1;
}