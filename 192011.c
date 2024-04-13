rtl8xxxu_alloc_tx_urb(struct rtl8xxxu_priv *priv)
{
	struct rtl8xxxu_tx_urb *tx_urb;
	unsigned long flags;

	spin_lock_irqsave(&priv->tx_urb_lock, flags);
	tx_urb = list_first_entry_or_null(&priv->tx_urb_free_list,
					  struct rtl8xxxu_tx_urb, list);
	if (tx_urb) {
		list_del(&tx_urb->list);
		priv->tx_urb_free_count--;
		if (priv->tx_urb_free_count < RTL8XXXU_TX_URB_LOW_WATER &&
		    !priv->tx_stopped) {
			priv->tx_stopped = true;
			ieee80211_stop_queues(priv->hw);
		}
	}

	spin_unlock_irqrestore(&priv->tx_urb_lock, flags);

	return tx_urb;
}