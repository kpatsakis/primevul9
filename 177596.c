static void isdn_net_softint(struct work_struct *work)
{
	isdn_net_local *lp = container_of(work, isdn_net_local, tqueue);
	struct sk_buff *skb;

	spin_lock_bh(&lp->xmit_lock);
	while (!isdn_net_lp_busy(lp)) {
		skb = skb_dequeue(&lp->super_tx_queue);
		if (!skb)
			break;
		isdn_net_writebuf_skb(lp, skb);
	}
	spin_unlock_bh(&lp->xmit_lock);
}