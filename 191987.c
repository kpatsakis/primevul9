static void rtl8xxxu_free_tx_resources(struct rtl8xxxu_priv *priv)
{
	struct rtl8xxxu_tx_urb *tx_urb, *tmp;
	unsigned long flags;

	spin_lock_irqsave(&priv->tx_urb_lock, flags);
	list_for_each_entry_safe(tx_urb, tmp, &priv->tx_urb_free_list, list) {
		list_del(&tx_urb->list);
		priv->tx_urb_free_count--;
		usb_free_urb(&tx_urb->urb);
	}
	spin_unlock_irqrestore(&priv->tx_urb_lock, flags);
}