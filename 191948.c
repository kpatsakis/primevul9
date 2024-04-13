static void rtl8xxxu_tx_complete(struct urb *urb)
{
	struct sk_buff *skb = (struct sk_buff *)urb->context;
	struct ieee80211_tx_info *tx_info;
	struct ieee80211_hw *hw;
	struct rtl8xxxu_priv *priv;
	struct rtl8xxxu_tx_urb *tx_urb =
		container_of(urb, struct rtl8xxxu_tx_urb, urb);

	tx_info = IEEE80211_SKB_CB(skb);
	hw = tx_info->rate_driver_data[0];
	priv = hw->priv;

	skb_pull(skb, priv->fops->tx_desc_size);

	ieee80211_tx_info_clear_status(tx_info);
	tx_info->status.rates[0].idx = -1;
	tx_info->status.rates[0].count = 0;

	if (!urb->status)
		tx_info->flags |= IEEE80211_TX_STAT_ACK;

	ieee80211_tx_status_irqsafe(hw, skb);

	rtl8xxxu_free_tx_urb(priv, tx_urb);
}