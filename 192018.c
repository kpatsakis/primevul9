static void rtl8xxxu_configure_filter(struct ieee80211_hw *hw,
				      unsigned int changed_flags,
				      unsigned int *total_flags, u64 multicast)
{
	struct rtl8xxxu_priv *priv = hw->priv;
	u32 rcr = rtl8xxxu_read32(priv, REG_RCR);

	dev_dbg(&priv->udev->dev, "%s: changed_flags %08x, total_flags %08x\n",
		__func__, changed_flags, *total_flags);

	/*
	 * FIF_ALLMULTI ignored as all multicast frames are accepted (REG_MAR)
	 */

	if (*total_flags & FIF_FCSFAIL)
		rcr |= RCR_ACCEPT_CRC32;
	else
		rcr &= ~RCR_ACCEPT_CRC32;

	/*
	 * FIF_PLCPFAIL not supported?
	 */

	if (*total_flags & FIF_BCN_PRBRESP_PROMISC)
		rcr &= ~RCR_CHECK_BSSID_BEACON;
	else
		rcr |= RCR_CHECK_BSSID_BEACON;

	if (*total_flags & FIF_CONTROL)
		rcr |= RCR_ACCEPT_CTRL_FRAME;
	else
		rcr &= ~RCR_ACCEPT_CTRL_FRAME;

	if (*total_flags & FIF_OTHER_BSS) {
		rcr |= RCR_ACCEPT_AP;
		rcr &= ~RCR_CHECK_BSSID_MATCH;
	} else {
		rcr &= ~RCR_ACCEPT_AP;
		rcr |= RCR_CHECK_BSSID_MATCH;
	}

	if (*total_flags & FIF_PSPOLL)
		rcr |= RCR_ACCEPT_PM;
	else
		rcr &= ~RCR_ACCEPT_PM;

	/*
	 * FIF_PROBE_REQ ignored as probe requests always seem to be accepted
	 */

	rtl8xxxu_write32(priv, REG_RCR, rcr);

	*total_flags &= (FIF_ALLMULTI | FIF_FCSFAIL | FIF_BCN_PRBRESP_PROMISC |
			 FIF_CONTROL | FIF_OTHER_BSS | FIF_PSPOLL |
			 FIF_PROBE_REQ);
}