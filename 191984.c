rtl8xxxu_fill_txdesc_v2(struct ieee80211_hw *hw, struct ieee80211_hdr *hdr,
			struct ieee80211_tx_info *tx_info,
			struct rtl8xxxu_txdesc32 *tx_desc32, bool sgi,
			bool short_preamble, bool ampdu_enable, u32 rts_rate)
{
	struct ieee80211_rate *tx_rate = ieee80211_get_tx_rate(hw, tx_info);
	struct rtl8xxxu_priv *priv = hw->priv;
	struct device *dev = &priv->udev->dev;
	struct rtl8xxxu_txdesc40 *tx_desc40;
	u32 rate;
	u16 rate_flags = tx_info->control.rates[0].flags;
	u16 seq_number;

	tx_desc40 = (struct rtl8xxxu_txdesc40 *)tx_desc32;

	if (rate_flags & IEEE80211_TX_RC_MCS &&
	    !ieee80211_is_mgmt(hdr->frame_control))
		rate = tx_info->control.rates[0].idx + DESC_RATE_MCS0;
	else
		rate = tx_rate->hw_value;

	if (rtl8xxxu_debug & RTL8XXXU_DEBUG_TX)
		dev_info(dev, "%s: TX rate: %d, pkt size %d\n",
			 __func__, rate, cpu_to_le16(tx_desc40->pkt_size));

	seq_number = IEEE80211_SEQ_TO_SN(le16_to_cpu(hdr->seq_ctrl));

	tx_desc40->txdw4 = cpu_to_le32(rate);
	if (ieee80211_is_data(hdr->frame_control)) {
		tx_desc40->txdw4 |= cpu_to_le32(0x1f <<
						TXDESC40_DATA_RATE_FB_SHIFT);
	}

	tx_desc40->txdw9 = cpu_to_le32((u32)seq_number << TXDESC40_SEQ_SHIFT);

	if (ampdu_enable)
		tx_desc40->txdw2 |= cpu_to_le32(TXDESC40_AGG_ENABLE);
	else
		tx_desc40->txdw2 |= cpu_to_le32(TXDESC40_AGG_BREAK);

	if (ieee80211_is_mgmt(hdr->frame_control)) {
		tx_desc40->txdw4 = cpu_to_le32(rate);
		tx_desc40->txdw3 |= cpu_to_le32(TXDESC40_USE_DRIVER_RATE);
		tx_desc40->txdw4 |=
			cpu_to_le32(6 << TXDESC40_RETRY_LIMIT_SHIFT);
		tx_desc40->txdw4 |= cpu_to_le32(TXDESC40_RETRY_LIMIT_ENABLE);
	}

	if (short_preamble)
		tx_desc40->txdw5 |= cpu_to_le32(TXDESC40_SHORT_PREAMBLE);

	tx_desc40->txdw4 |= cpu_to_le32(rts_rate << TXDESC40_RTS_RATE_SHIFT);
	/*
	 * rts_rate is zero if RTS/CTS or CTS to SELF are not enabled
	 */
	if (rate_flags & IEEE80211_TX_RC_USE_RTS_CTS) {
		tx_desc40->txdw3 |= cpu_to_le32(TXDESC40_RTS_CTS_ENABLE);
		tx_desc40->txdw3 |= cpu_to_le32(TXDESC40_HW_RTS_ENABLE);
	} else if (rate_flags & IEEE80211_TX_RC_USE_CTS_PROTECT) {
		/*
		 * For some reason the vendor driver doesn't set
		 * TXDESC40_HW_RTS_ENABLE for CTS to SELF
		 */
		tx_desc40->txdw3 |= cpu_to_le32(TXDESC40_CTS_SELF_ENABLE);
	}
}