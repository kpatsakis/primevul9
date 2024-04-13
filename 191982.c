static void rtl8xxxu_tx(struct ieee80211_hw *hw,
			struct ieee80211_tx_control *control,
			struct sk_buff *skb)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)skb->data;
	struct ieee80211_tx_info *tx_info = IEEE80211_SKB_CB(skb);
	struct rtl8xxxu_priv *priv = hw->priv;
	struct rtl8xxxu_txdesc32 *tx_desc;
	struct rtl8xxxu_tx_urb *tx_urb;
	struct ieee80211_sta *sta = NULL;
	struct ieee80211_vif *vif = tx_info->control.vif;
	struct device *dev = &priv->udev->dev;
	u32 queue, rts_rate;
	u16 pktlen = skb->len;
	u16 rate_flag = tx_info->control.rates[0].flags;
	int tx_desc_size = priv->fops->tx_desc_size;
	int ret;
	bool ampdu_enable, sgi = false, short_preamble = false;

	if (skb_headroom(skb) < tx_desc_size) {
		dev_warn(dev,
			 "%s: Not enough headroom (%i) for tx descriptor\n",
			 __func__, skb_headroom(skb));
		goto error;
	}

	if (unlikely(skb->len > (65535 - tx_desc_size))) {
		dev_warn(dev, "%s: Trying to send over-sized skb (%i)\n",
			 __func__, skb->len);
		goto error;
	}

	tx_urb = rtl8xxxu_alloc_tx_urb(priv);
	if (!tx_urb) {
		dev_warn(dev, "%s: Unable to allocate tx urb\n", __func__);
		goto error;
	}

	if (ieee80211_is_action(hdr->frame_control))
		rtl8xxxu_dump_action(dev, hdr);

	tx_info->rate_driver_data[0] = hw;

	if (control && control->sta)
		sta = control->sta;

	tx_desc = skb_push(skb, tx_desc_size);

	memset(tx_desc, 0, tx_desc_size);
	tx_desc->pkt_size = cpu_to_le16(pktlen);
	tx_desc->pkt_offset = tx_desc_size;

	tx_desc->txdw0 =
		TXDESC_OWN | TXDESC_FIRST_SEGMENT | TXDESC_LAST_SEGMENT;
	if (is_multicast_ether_addr(ieee80211_get_DA(hdr)) ||
	    is_broadcast_ether_addr(ieee80211_get_DA(hdr)))
		tx_desc->txdw0 |= TXDESC_BROADMULTICAST;

	queue = rtl8xxxu_queue_select(hw, skb);
	tx_desc->txdw1 = cpu_to_le32(queue << TXDESC_QUEUE_SHIFT);

	if (tx_info->control.hw_key) {
		switch (tx_info->control.hw_key->cipher) {
		case WLAN_CIPHER_SUITE_WEP40:
		case WLAN_CIPHER_SUITE_WEP104:
		case WLAN_CIPHER_SUITE_TKIP:
			tx_desc->txdw1 |= cpu_to_le32(TXDESC_SEC_RC4);
			break;
		case WLAN_CIPHER_SUITE_CCMP:
			tx_desc->txdw1 |= cpu_to_le32(TXDESC_SEC_AES);
			break;
		default:
			break;
		}
	}

	/* (tx_info->flags & IEEE80211_TX_CTL_AMPDU) && */
	ampdu_enable = false;
	if (ieee80211_is_data_qos(hdr->frame_control) && sta) {
		if (sta->ht_cap.ht_supported) {
			u32 ampdu, val32;

			ampdu = (u32)sta->ht_cap.ampdu_density;
			val32 = ampdu << TXDESC_AMPDU_DENSITY_SHIFT;
			tx_desc->txdw2 |= cpu_to_le32(val32);

			ampdu_enable = true;
		}
	}

	if (rate_flag & IEEE80211_TX_RC_SHORT_GI ||
	    (ieee80211_is_data_qos(hdr->frame_control) &&
	     sta && sta->ht_cap.cap &
	     (IEEE80211_HT_CAP_SGI_40 | IEEE80211_HT_CAP_SGI_20)))
		sgi = true;

	if (rate_flag & IEEE80211_TX_RC_USE_SHORT_PREAMBLE ||
	    (sta && vif && vif->bss_conf.use_short_preamble))
		short_preamble = true;

	if (rate_flag & IEEE80211_TX_RC_USE_RTS_CTS)
		rts_rate = ieee80211_get_rts_cts_rate(hw, tx_info)->hw_value;
	else if (rate_flag & IEEE80211_TX_RC_USE_CTS_PROTECT)
		rts_rate = ieee80211_get_rts_cts_rate(hw, tx_info)->hw_value;
	else
		rts_rate = 0;


	priv->fops->fill_txdesc(hw, hdr, tx_info, tx_desc, sgi, short_preamble,
				ampdu_enable, rts_rate);

	rtl8xxxu_calc_tx_desc_csum(tx_desc);

	usb_fill_bulk_urb(&tx_urb->urb, priv->udev, priv->pipe_out[queue],
			  skb->data, skb->len, rtl8xxxu_tx_complete, skb);

	usb_anchor_urb(&tx_urb->urb, &priv->tx_anchor);
	ret = usb_submit_urb(&tx_urb->urb, GFP_ATOMIC);
	if (ret) {
		usb_unanchor_urb(&tx_urb->urb);
		rtl8xxxu_free_tx_urb(priv, tx_urb);
		goto error;
	}
	return;
error:
	dev_kfree_skb(skb);
}