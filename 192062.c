static int rtl8xxxu_conf_tx(struct ieee80211_hw *hw,
			    struct ieee80211_vif *vif, u16 queue,
			    const struct ieee80211_tx_queue_params *param)
{
	struct rtl8xxxu_priv *priv = hw->priv;
	struct device *dev = &priv->udev->dev;
	u32 val32;
	u8 aifs, acm_ctrl, acm_bit;

	aifs = param->aifs;

	val32 = aifs |
		fls(param->cw_min) << EDCA_PARAM_ECW_MIN_SHIFT |
		fls(param->cw_max) << EDCA_PARAM_ECW_MAX_SHIFT |
		(u32)param->txop << EDCA_PARAM_TXOP_SHIFT;

	acm_ctrl = rtl8xxxu_read8(priv, REG_ACM_HW_CTRL);
	dev_dbg(dev,
		"%s: IEEE80211 queue %02x val %08x, acm %i, acm_ctrl %02x\n",
		__func__, queue, val32, param->acm, acm_ctrl);

	switch (queue) {
	case IEEE80211_AC_VO:
		acm_bit = ACM_HW_CTRL_VO;
		rtl8xxxu_write32(priv, REG_EDCA_VO_PARAM, val32);
		break;
	case IEEE80211_AC_VI:
		acm_bit = ACM_HW_CTRL_VI;
		rtl8xxxu_write32(priv, REG_EDCA_VI_PARAM, val32);
		break;
	case IEEE80211_AC_BE:
		acm_bit = ACM_HW_CTRL_BE;
		rtl8xxxu_write32(priv, REG_EDCA_BE_PARAM, val32);
		break;
	case IEEE80211_AC_BK:
		acm_bit = ACM_HW_CTRL_BK;
		rtl8xxxu_write32(priv, REG_EDCA_BK_PARAM, val32);
		break;
	default:
		acm_bit = 0;
		break;
	}

	if (param->acm)
		acm_ctrl |= acm_bit;
	else
		acm_ctrl &= ~acm_bit;
	rtl8xxxu_write8(priv, REG_ACM_HW_CTRL, acm_ctrl);

	return 0;
}