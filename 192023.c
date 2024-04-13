static void rtl8xxxu_free_rx_resources(struct rtl8xxxu_priv *priv)
{
	struct rtl8xxxu_rx_urb *rx_urb, *tmp;
	unsigned long flags;

	spin_lock_irqsave(&priv->rx_urb_lock, flags);

	list_for_each_entry_safe(rx_urb, tmp,
				 &priv->rx_urb_pending_list, list) {
		list_del(&rx_urb->list);
		priv->rx_urb_pending_count--;
		usb_free_urb(&rx_urb->urb);
	}

	spin_unlock_irqrestore(&priv->rx_urb_lock, flags);
}