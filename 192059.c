static void rtl8xxxu_rx_complete(struct urb *urb)
{
	struct rtl8xxxu_rx_urb *rx_urb =
		container_of(urb, struct rtl8xxxu_rx_urb, urb);
	struct ieee80211_hw *hw = rx_urb->hw;
	struct rtl8xxxu_priv *priv = hw->priv;
	struct sk_buff *skb = (struct sk_buff *)urb->context;
	struct device *dev = &priv->udev->dev;

	skb_put(skb, urb->actual_length);

	if (urb->status == 0) {
		priv->fops->parse_rx_desc(priv, skb);

		skb = NULL;
		rx_urb->urb.context = NULL;
		rtl8xxxu_queue_rx_urb(priv, rx_urb);
	} else {
		dev_dbg(dev, "%s: status %i\n",	__func__, urb->status);
		goto cleanup;
	}
	return;

cleanup:
	usb_free_urb(urb);
	dev_kfree_skb(skb);
	return;
}