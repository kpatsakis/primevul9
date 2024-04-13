int rtl8xxxu_parse_rxdesc16(struct rtl8xxxu_priv *priv, struct sk_buff *skb)
{
	struct ieee80211_hw *hw = priv->hw;
	struct ieee80211_rx_status *rx_status;
	struct rtl8xxxu_rxdesc16 *rx_desc;
	struct rtl8723au_phy_stats *phy_stats;
	struct sk_buff *next_skb = NULL;
	__le32 *_rx_desc_le;
	u32 *_rx_desc;
	int drvinfo_sz, desc_shift;
	int i, pkt_cnt, pkt_len, urb_len, pkt_offset;

	urb_len = skb->len;
	pkt_cnt = 0;

	do {
		rx_desc = (struct rtl8xxxu_rxdesc16 *)skb->data;
		_rx_desc_le = (__le32 *)skb->data;
		_rx_desc = (u32 *)skb->data;

		for (i = 0;
		     i < (sizeof(struct rtl8xxxu_rxdesc16) / sizeof(u32)); i++)
			_rx_desc[i] = le32_to_cpu(_rx_desc_le[i]);

		/*
		 * Only read pkt_cnt from the header if we're parsing the
		 * first packet
		 */
		if (!pkt_cnt)
			pkt_cnt = rx_desc->pkt_cnt;
		pkt_len = rx_desc->pktlen;

		drvinfo_sz = rx_desc->drvinfo_sz * 8;
		desc_shift = rx_desc->shift;
		pkt_offset = roundup(pkt_len + drvinfo_sz + desc_shift +
				     sizeof(struct rtl8xxxu_rxdesc16), 128);

		/*
		 * Only clone the skb if there's enough data at the end to
		 * at least cover the rx descriptor
		 */
		if (pkt_cnt > 1 &&
		    urb_len > (pkt_offset + sizeof(struct rtl8xxxu_rxdesc16)))
			next_skb = skb_clone(skb, GFP_ATOMIC);

		rx_status = IEEE80211_SKB_RXCB(skb);
		memset(rx_status, 0, sizeof(struct ieee80211_rx_status));

		skb_pull(skb, sizeof(struct rtl8xxxu_rxdesc16));

		phy_stats = (struct rtl8723au_phy_stats *)skb->data;

		skb_pull(skb, drvinfo_sz + desc_shift);

		skb_trim(skb, pkt_len);

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

		if (rx_desc->rxht) {
			rx_status->encoding = RX_ENC_HT;
			rx_status->rate_idx = rx_desc->rxmcs - DESC_RATE_MCS0;
		} else {
			rx_status->rate_idx = rx_desc->rxmcs;
		}

		rx_status->freq = hw->conf.chandef.chan->center_freq;
		rx_status->band = hw->conf.chandef.chan->band;

		ieee80211_rx_irqsafe(hw, skb);

		skb = next_skb;
		if (skb)
			skb_pull(next_skb, pkt_offset);

		pkt_cnt--;
		urb_len -= pkt_offset;
	} while (skb && urb_len > 0 && pkt_cnt > 0);

	return RX_TYPE_DATA_PKT;
}