static void isdn_net_tx_timeout(struct net_device *ndev)
{
	isdn_net_local *lp = netdev_priv(ndev);

	printk(KERN_WARNING "isdn_tx_timeout dev %s dialstate %d\n", ndev->name, lp->dialstate);
	if (!lp->dialstate) {
		lp->stats.tx_errors++;
		/*
		 * There is a certain probability that this currently
		 * works at all because if we always wake up the interface,
		 * then upper layer will try to send the next packet
		 * immediately. And then, the old clean_up logic in the
		 * driver will hopefully continue to work as it used to do.
		 *
		 * This is rather primitive right know, we better should
		 * clean internal queues here, in particular for multilink and
		 * ppp, and reset HL driver's channel, too.   --HE
		 *
		 * actually, this may not matter at all, because ISDN hardware
		 * should not see transmitter hangs at all IMO
		 * changed KERN_DEBUG to KERN_WARNING to find out if this is
		 * ever called   --KG
		 */
	}
	netif_trans_update(ndev);
	netif_wake_queue(ndev);
}