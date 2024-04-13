int rtl8xxxu_parse_rxdesc24(struct rtl8xxxu_priv *priv, struct sk_buff *skb)
{
	struct ieee80211_hw *hw = priv->hw;
	struct ieee80211_rx_status *rx_status = IEEE80211_SKB_RXCB(skb);
	struct rtl8xxxu_rxdesc24 *rx_desc =
		(struct rtl8xxxu_rxdesc24 *)skb->data;
	struct rtl8723au_phy_stats *phy_stats;
	__le32 *_rx_desc_le = (__le32 *)skb->data;
	u32 *_rx_desc = (u32 *)skb->data;
	int drvinfo_sz, desc_shift;
	int i;

	for (i = 0; i < (sizeof(struct rtl8xxxu_rxdesc24) / sizeof(u32)); i++)
		_rx_desc[i] = le32_to_cpu(_rx_desc_le[i]);

	memset(rx_status, 0, sizeof(struct ieee80211_rx_status));

	skb_pull(skb, sizeof(struct rtl8xxxu_rxdesc24));

	phy_stats = (struct rtl8723au_phy_stats *)skb->data;

	drvinfo_sz = rx_desc->drvinfo_sz * 8;
	desc_shift = rx_desc->shift;
	skb_pull(skb, drvinfo_sz + desc_shift);

	if (rx_desc->rpt_sel) {
		struct device *dev = &priv->udev->dev;
		dev_dbg(dev, "%s: C2H packet\n", __func__);
		rtl8723bu_handle_c2h(priv, skb);
		dev_kfree_skb(skb);
		return RX_TYPE_C2H;
	}

	if (rx_desc->phy_stats)
		rtl8xxxu_rx_parse_phystats(priv, rx_status, phy_stats,
					   rx_desc->rxmcs);

	rx_status->mactime = rx_desc->tsfl;
	rx_status->flag |= RX_FLAG_MACTIME_START;

	if (!rx_desc->swdec)
		rx_status->flag |= RX_FLAG_DECRYPTED;
	if (rx_desc->crc32)
		rx_status->flag |= RX_FLAG_FAILED_FCS_CRC;
	if (rx_desc->bw)
		rx_status->bw = RATE_INFO_BW_40;

	if (rx_desc->rxmcs >= DESC_RATE_MCS0) {
		rx_status->encoding = RX_ENC_HT;
		rx_status->rate_idx = rx_desc->rxmcs - DESC_RATE_MCS0;
	} else {
		rx_status->rate_idx = rx_desc->rxmcs;
	}

	rx_status->freq = hw->conf.chandef.chan->center_freq;
	rx_status->band = hw->conf.chandef.chan->band;

	ieee80211_rx_irqsafe(hw, skb);
	return RX_TYPE_DATA_PKT;
}