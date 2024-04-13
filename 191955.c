rtl8xxxu_bss_info_changed(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
			  struct ieee80211_bss_conf *bss_conf, u32 changed)
{
	struct rtl8xxxu_priv *priv = hw->priv;
	struct device *dev = &priv->udev->dev;
	struct ieee80211_sta *sta;
	u32 val32;
	u8 val8;

	if (changed & BSS_CHANGED_ASSOC) {
		dev_dbg(dev, "Changed ASSOC: %i!\n", bss_conf->assoc);

		rtl8xxxu_set_linktype(priv, vif->type);

		if (bss_conf->assoc) {
			u32 ramask;
			int sgi = 0;

			rcu_read_lock();
			sta = ieee80211_find_sta(vif, bss_conf->bssid);
			if (!sta) {
				dev_info(dev, "%s: ASSOC no sta found\n",
					 __func__);
				rcu_read_unlock();
				goto error;
			}

			if (sta->ht_cap.ht_supported)
				dev_info(dev, "%s: HT supported\n", __func__);
			if (sta->vht_cap.vht_supported)
				dev_info(dev, "%s: VHT supported\n", __func__);

			/* TODO: Set bits 28-31 for rate adaptive id */
			ramask = (sta->supp_rates[0] & 0xfff) |
				sta->ht_cap.mcs.rx_mask[0] << 12 |
				sta->ht_cap.mcs.rx_mask[1] << 20;
			if (sta->ht_cap.cap &
			    (IEEE80211_HT_CAP_SGI_40 | IEEE80211_HT_CAP_SGI_20))
				sgi = 1;
			rcu_read_unlock();

			priv->fops->update_rate_mask(priv, ramask, sgi);

			rtl8xxxu_write8(priv, REG_BCN_MAX_ERR, 0xff);

			rtl8xxxu_stop_tx_beacon(priv);

			/* joinbss sequence */
			rtl8xxxu_write16(priv, REG_BCN_PSR_RPT,
					 0xc000 | bss_conf->aid);

			priv->fops->report_connect(priv, 0, true);
		} else {
			val8 = rtl8xxxu_read8(priv, REG_BEACON_CTRL);
			val8 |= BEACON_DISABLE_TSF_UPDATE;
			rtl8xxxu_write8(priv, REG_BEACON_CTRL, val8);

			priv->fops->report_connect(priv, 0, false);
		}
	}

	if (changed & BSS_CHANGED_ERP_PREAMBLE) {
		dev_dbg(dev, "Changed ERP_PREAMBLE: Use short preamble %i\n",
			bss_conf->use_short_preamble);
		val32 = rtl8xxxu_read32(priv, REG_RESPONSE_RATE_SET);
		if (bss_conf->use_short_preamble)
			val32 |= RSR_ACK_SHORT_PREAMBLE;
		else
			val32 &= ~RSR_ACK_SHORT_PREAMBLE;
		rtl8xxxu_write32(priv, REG_RESPONSE_RATE_SET, val32);
	}

	if (changed & BSS_CHANGED_ERP_SLOT) {
		dev_dbg(dev, "Changed ERP_SLOT: short_slot_time %i\n",
			bss_conf->use_short_slot);

		if (bss_conf->use_short_slot)
			val8 = 9;
		else
			val8 = 20;
		rtl8xxxu_write8(priv, REG_SLOT, val8);
	}

	if (changed & BSS_CHANGED_BSSID) {
		dev_dbg(dev, "Changed BSSID!\n");
		rtl8xxxu_set_bssid(priv, bss_conf->bssid);
	}

	if (changed & BSS_CHANGED_BASIC_RATES) {
		dev_dbg(dev, "Changed BASIC_RATES!\n");
		rtl8xxxu_set_basic_rates(priv, bss_conf->basic_rates);
	}
error:
	return;
}