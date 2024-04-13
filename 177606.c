void isdn_net_writebuf_skb(isdn_net_local *lp, struct sk_buff *skb)
{
	int ret;
	int len = skb->len;     /* save len */

	/* before obtaining the lock the caller should have checked that
	   the lp isn't busy */
	if (isdn_net_lp_busy(lp)) {
		printk("isdn BUG at %s:%d!\n", __FILE__, __LINE__);
		goto error;
	}

	if (!(lp->flags & ISDN_NET_CONNECTED)) {
		printk("isdn BUG at %s:%d!\n", __FILE__, __LINE__);
		goto error;
	}
	ret = isdn_writebuf_skb_stub(lp->isdn_device, lp->isdn_channel, 1, skb);
	if (ret != len) {
		/* we should never get here */
		printk(KERN_WARNING "%s: HL driver queue full\n", lp->netdev->dev->name);
		goto error;
	}

	lp->transcount += len;
	isdn_net_inc_frame_cnt(lp);
	return;

error:
	dev_kfree_skb(skb);
	lp->stats.tx_errors++;

}