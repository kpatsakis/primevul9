static int rtl8xxxu_set_key(struct ieee80211_hw *hw, enum set_key_cmd cmd,
			    struct ieee80211_vif *vif,
			    struct ieee80211_sta *sta,
			    struct ieee80211_key_conf *key)
{
	struct rtl8xxxu_priv *priv = hw->priv;
	struct device *dev = &priv->udev->dev;
	u8 mac_addr[ETH_ALEN];
	u8 val8;
	u16 val16;
	u32 val32;
	int retval = -EOPNOTSUPP;

	dev_dbg(dev, "%s: cmd %02x, cipher %08x, index %i\n",
		__func__, cmd, key->cipher, key->keyidx);

	if (vif->type != NL80211_IFTYPE_STATION)
		return -EOPNOTSUPP;

	if (key->keyidx > 3)
		return -EOPNOTSUPP;

	switch (key->cipher) {
	case WLAN_CIPHER_SUITE_WEP40:
	case WLAN_CIPHER_SUITE_WEP104:

		break;
	case WLAN_CIPHER_SUITE_CCMP:
		key->flags |= IEEE80211_KEY_FLAG_SW_MGMT_TX;
		break;
	case WLAN_CIPHER_SUITE_TKIP:
		key->flags |= IEEE80211_KEY_FLAG_GENERATE_MMIC;
		break;
	default:
		return -EOPNOTSUPP;
	}

	if (key->flags & IEEE80211_KEY_FLAG_PAIRWISE) {
		dev_dbg(dev, "%s: pairwise key\n", __func__);
		ether_addr_copy(mac_addr, sta->addr);
	} else {
		dev_dbg(dev, "%s: group key\n", __func__);
		eth_broadcast_addr(mac_addr);
	}

	val16 = rtl8xxxu_read16(priv, REG_CR);
	val16 |= CR_SECURITY_ENABLE;
	rtl8xxxu_write16(priv, REG_CR, val16);

	val8 = SEC_CFG_TX_SEC_ENABLE | SEC_CFG_TXBC_USE_DEFKEY |
		SEC_CFG_RX_SEC_ENABLE | SEC_CFG_RXBC_USE_DEFKEY;
	val8 |= SEC_CFG_TX_USE_DEFKEY | SEC_CFG_RX_USE_DEFKEY;
	rtl8xxxu_write8(priv, REG_SECURITY_CFG, val8);

	switch (cmd) {
	case SET_KEY:
		key->hw_key_idx = key->keyidx;
		key->flags |= IEEE80211_KEY_FLAG_GENERATE_IV;
		rtl8xxxu_cam_write(priv, key, mac_addr);
		retval = 0;
		break;
	case DISABLE_KEY:
		rtl8xxxu_write32(priv, REG_CAM_WRITE, 0x00000000);
		val32 = CAM_CMD_POLLING | CAM_CMD_WRITE |
			key->keyidx << CAM_CMD_KEY_SHIFT;
		rtl8xxxu_write32(priv, REG_CAM_CMD, val32);
		retval = 0;
		break;
	default:
		dev_warn(dev, "%s: Unsupported command %02x\n", __func__, cmd);
	}

	return retval;
}