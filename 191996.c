static void rtl8xxxu_free_tx_urb(struct rtl8xxxu_priv *priv,
				 struct rtl8xxxu_tx_urb *tx_urb)
{
	unsigned long flags;

	INIT_LIST_HEAD(&tx_urb->list);

	spin_lock_irqsave(&priv->tx_urb_lock, flags);

	list_add(&tx_urb->list, &priv->tx_urb_free_list);
	priv->tx_urb_free_count++;
	if (priv->tx_urb_free_count > RTL8XXXU_TX_URB_HIGH_WATER &&
	    priv->tx_stopped) {
		priv->tx_stopped = false;
		ieee80211_wake_queues(priv->hw);
	}

	spin_unlock_irqrestore(&priv->tx_urb_lock, flags);
}