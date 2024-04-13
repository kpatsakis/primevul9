static void rtl8xxxu_rx_urb_work(struct work_struct *work)
{
	struct rtl8xxxu_priv *priv;
	struct rtl8xxxu_rx_urb *rx_urb, *tmp;
	struct list_head local;
	struct sk_buff *skb;
	unsigned long flags;
	int ret;

	priv = container_of(work, struct rtl8xxxu_priv, rx_urb_wq);
	INIT_LIST_HEAD(&local);

	spin_lock_irqsave(&priv->rx_urb_lock, flags);

	list_splice_init(&priv->rx_urb_pending_list, &local);
	priv->rx_urb_pending_count = 0;

	spin_unlock_irqrestore(&priv->rx_urb_lock, flags);

	list_for_each_entry_safe(rx_urb, tmp, &local, list) {
		list_del_init(&rx_urb->list);
		ret = rtl8xxxu_submit_rx_urb(priv, rx_urb);
		/*
		 * If out of memory or temporary error, put it back on the
		 * queue and try again. Otherwise the device is dead/gone
		 * and we should drop it.
		 */
		switch (ret) {
		case 0:
			break;
		case -ENOMEM:
		case -EAGAIN:
			rtl8xxxu_queue_rx_urb(priv, rx_urb);
			break;
		default:
			pr_info("failed to requeue urb %i\n", ret);
			skb = (struct sk_buff *)rx_urb->urb.context;
			dev_kfree_skb(skb);
			usb_free_urb(&rx_urb->urb);
		}
	}
}