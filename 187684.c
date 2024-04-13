static void yam_arbitrate(struct net_device *dev)
{
	struct yam_port *yp = netdev_priv(dev);

	if (yp->magic != YAM_MAGIC || yp->tx_state != TX_OFF ||
	    skb_queue_empty(&yp->send_queue))
		return;
	/* tx_state is TX_OFF and there is data to send */

	if (yp->dupmode) {
		/* Full duplex mode, don't wait */
		yam_start_tx(dev, yp);
		return;
	}
	if (yp->dcd) {
		/* DCD on, wait slotime ... */
		yp->slotcnt = yp->slot / 10;
		return;
	}
	/* Is slottime passed ? */
	if ((--yp->slotcnt) > 0)
		return;

	yp->slotcnt = yp->slot / 10;

	/* is random > persist ? */
	if ((prandom_u32() % 256) > yp->pers)
		return;

	yam_start_tx(dev, yp);
}