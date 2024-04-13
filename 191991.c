static int rtl8xxxu_add_interface(struct ieee80211_hw *hw,
				  struct ieee80211_vif *vif)
{
	struct rtl8xxxu_priv *priv = hw->priv;
	int ret;
	u8 val8;

	switch (vif->type) {
	case NL80211_IFTYPE_STATION:
		rtl8xxxu_stop_tx_beacon(priv);

		val8 = rtl8xxxu_read8(priv, REG_BEACON_CTRL);
		val8 |= BEACON_ATIM | BEACON_FUNCTION_ENABLE |
			BEACON_DISABLE_TSF_UPDATE;
		rtl8xxxu_write8(priv, REG_BEACON_CTRL, val8);
		ret = 0;
		break;
	default:
		ret = -EOPNOTSUPP;
	}

	rtl8xxxu_set_linktype(priv, vif->type);

	return ret;
}