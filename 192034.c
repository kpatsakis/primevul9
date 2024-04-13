static int rtl8xxxu_submit_rx_urb(struct rtl8xxxu_priv *priv,
				  struct rtl8xxxu_rx_urb *rx_urb)
{
	struct rtl8xxxu_fileops *fops = priv->fops;
	struct sk_buff *skb;
	int skb_size;
	int ret, rx_desc_sz;

	rx_desc_sz = fops->rx_desc_size;

	if (priv->rx_buf_aggregation && fops->rx_agg_buf_size) {
		skb_size = fops->rx_agg_buf_size;
		skb_size += (rx_desc_sz + sizeof(struct rtl8723au_phy_stats));
	} else {
		skb_size = IEEE80211_MAX_FRAME_LEN;
	}

	skb = __netdev_alloc_skb(NULL, skb_size, GFP_KERNEL);
	if (!skb)
		return -ENOMEM;

	memset(skb->data, 0, rx_desc_sz);
	usb_fill_bulk_urb(&rx_urb->urb, priv->udev, priv->pipe_in, skb->data,
			  skb_size, rtl8xxxu_rx_complete, skb);
	usb_anchor_urb(&rx_urb->urb, &priv->rx_anchor);
	ret = usb_submit_urb(&rx_urb->urb, GFP_ATOMIC);
	if (ret)
		usb_unanchor_urb(&rx_urb->urb);
	return ret;
}