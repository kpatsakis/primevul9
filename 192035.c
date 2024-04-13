static void rtl8xxxu_remove_interface(struct ieee80211_hw *hw,
				      struct ieee80211_vif *vif)
{
	struct rtl8xxxu_priv *priv = hw->priv;

	dev_dbg(&priv->udev->dev, "%s\n", __func__);
}