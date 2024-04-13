rtl8xxxu_ampdu_action(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
		      struct ieee80211_ampdu_params *params)
{
	struct rtl8xxxu_priv *priv = hw->priv;
	struct device *dev = &priv->udev->dev;
	u8 ampdu_factor, ampdu_density;
	struct ieee80211_sta *sta = params->sta;
	enum ieee80211_ampdu_mlme_action action = params->action;

	switch (action) {
	case IEEE80211_AMPDU_TX_START:
		dev_dbg(dev, "%s: IEEE80211_AMPDU_TX_START\n", __func__);
		ampdu_factor = sta->ht_cap.ampdu_factor;
		ampdu_density = sta->ht_cap.ampdu_density;
		rtl8xxxu_set_ampdu_factor(priv, ampdu_factor);
		rtl8xxxu_set_ampdu_min_space(priv, ampdu_density);
		dev_dbg(dev,
			"Changed HT: ampdu_factor %02x, ampdu_density %02x\n",
			ampdu_factor, ampdu_density);
		break;
	case IEEE80211_AMPDU_TX_STOP_FLUSH:
		dev_dbg(dev, "%s: IEEE80211_AMPDU_TX_STOP_FLUSH\n", __func__);
		rtl8xxxu_set_ampdu_factor(priv, 0);
		rtl8xxxu_set_ampdu_min_space(priv, 0);
		break;
	case IEEE80211_AMPDU_TX_STOP_FLUSH_CONT:
		dev_dbg(dev, "%s: IEEE80211_AMPDU_TX_STOP_FLUSH_CONT\n",
			 __func__);
		rtl8xxxu_set_ampdu_factor(priv, 0);
		rtl8xxxu_set_ampdu_min_space(priv, 0);
		break;
	case IEEE80211_AMPDU_RX_START:
		dev_dbg(dev, "%s: IEEE80211_AMPDU_RX_START\n", __func__);
		break;
	case IEEE80211_AMPDU_RX_STOP:
		dev_dbg(dev, "%s: IEEE80211_AMPDU_RX_STOP\n", __func__);
		break;
	default:
		break;
	}
	return 0;
}