static void rtl8xxxu_queue_rx_urb(struct rtl8xxxu_priv *priv,
				  struct rtl8xxxu_rx_urb *rx_urb)
{
	struct sk_buff *skb;
	unsigned long flags;
	int pending = 0;

	spin_lock_irqsave(&priv->rx_urb_lock, flags);

	if (!priv->shutdown) {
		list_add_tail(&rx_urb->list, &priv->rx_urb_pending_list);
		priv->rx_urb_pending_count++;
		pending = priv->rx_urb_pending_count;
	} else {
		skb = (struct sk_buff *)rx_urb->urb.context;
		dev_kfree_skb(skb);
		usb_free_urb(&rx_urb->urb);
	}

	spin_unlock_irqrestore(&priv->rx_urb_lock, flags);

	if (pending > RTL8XXXU_RX_URB_PENDING_WATER)
		schedule_work(&priv->rx_urb_wq);
}