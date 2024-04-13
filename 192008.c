static void rtl8xxxu_sw_scan_complete(struct ieee80211_hw *hw,
				      struct ieee80211_vif *vif)
{
	struct rtl8xxxu_priv *priv = hw->priv;
	u8 val8;

	val8 = rtl8xxxu_read8(priv, REG_BEACON_CTRL);
	val8 &= ~BEACON_DISABLE_TSF_UPDATE;
	rtl8xxxu_write8(priv, REG_BEACON_CTRL, val8);
}