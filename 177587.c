void isdn_net_write_super(isdn_net_local *lp, struct sk_buff *skb)
{
	if (in_irq()) {
		// we can't grab the lock from irq context,
		// so we just queue the packet
		skb_queue_tail(&lp->super_tx_queue, skb);
		schedule_work(&lp->tqueue);
		return;
	}

	spin_lock_bh(&lp->xmit_lock);
	if (!isdn_net_lp_busy(lp)) {
		isdn_net_writebuf_skb(lp, skb);
	} else {
		skb_queue_tail(&lp->super_tx_queue, skb);
	}
	spin_unlock_bh(&lp->xmit_lock);
}