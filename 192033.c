static int rtl8xxxu_config(struct ieee80211_hw *hw, u32 changed)
{
	struct rtl8xxxu_priv *priv = hw->priv;
	struct device *dev = &priv->udev->dev;
	u16 val16;
	int ret = 0, channel;
	bool ht40;

	if (rtl8xxxu_debug & RTL8XXXU_DEBUG_CHANNEL)
		dev_info(dev,
			 "%s: channel: %i (changed %08x chandef.width %02x)\n",
			 __func__, hw->conf.chandef.chan->hw_value,
			 changed, hw->conf.chandef.width);

	if (changed & IEEE80211_CONF_CHANGE_RETRY_LIMITS) {
		val16 = ((hw->conf.long_frame_max_tx_count <<
			  RETRY_LIMIT_LONG_SHIFT) & RETRY_LIMIT_LONG_MASK) |
			((hw->conf.short_frame_max_tx_count <<
			  RETRY_LIMIT_SHORT_SHIFT) & RETRY_LIMIT_SHORT_MASK);
		rtl8xxxu_write16(priv, REG_RETRY_LIMIT, val16);
	}

	if (changed & IEEE80211_CONF_CHANGE_CHANNEL) {
		switch (hw->conf.chandef.width) {
		case NL80211_CHAN_WIDTH_20_NOHT:
		case NL80211_CHAN_WIDTH_20:
			ht40 = false;
			break;
		case NL80211_CHAN_WIDTH_40:
			ht40 = true;
			break;
		default:
			ret = -ENOTSUPP;
			goto exit;
		}

		channel = hw->conf.chandef.chan->hw_value;

		priv->fops->set_tx_power(priv, channel, ht40);

		priv->fops->config_channel(hw);
	}

exit:
	return ret;
}