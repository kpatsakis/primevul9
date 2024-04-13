int rtl8xxxu_active_to_lps(struct rtl8xxxu_priv *priv)
{
	u8 val8;
	u8 val32;
	int count, ret = 0;

	rtl8xxxu_write8(priv, REG_TXPAUSE, 0xff);

	/*
	 * Poll - wait for RX packet to complete
	 */
	for (count = RTL8XXXU_MAX_REG_POLL; count; count--) {
		val32 = rtl8xxxu_read32(priv, 0x5f8);
		if (!val32)
			break;
		udelay(10);
	}

	if (!count) {
		dev_warn(&priv->udev->dev,
			 "%s: RX poll timed out (0x05f8)\n", __func__);
		ret = -EBUSY;
		goto exit;
	}

	/* Disable CCK and OFDM, clock gated */
	val8 = rtl8xxxu_read8(priv, REG_SYS_FUNC);
	val8 &= ~SYS_FUNC_BBRSTB;
	rtl8xxxu_write8(priv, REG_SYS_FUNC, val8);

	udelay(2);

	/* Reset baseband */
	val8 = rtl8xxxu_read8(priv, REG_SYS_FUNC);
	val8 &= ~SYS_FUNC_BB_GLB_RSTN;
	rtl8xxxu_write8(priv, REG_SYS_FUNC, val8);

	/* Reset MAC TRX */
	val8 = rtl8xxxu_read8(priv, REG_CR);
	val8 = CR_HCI_TXDMA_ENABLE | CR_HCI_RXDMA_ENABLE;
	rtl8xxxu_write8(priv, REG_CR, val8);

	/* Reset MAC TRX */
	val8 = rtl8xxxu_read8(priv, REG_CR + 1);
	val8 &= ~BIT(1); /* CR_SECURITY_ENABLE */
	rtl8xxxu_write8(priv, REG_CR + 1, val8);

	/* Respond TX OK to scheduler */
	val8 = rtl8xxxu_read8(priv, REG_DUAL_TSF_RST);
	val8 |= DUAL_TSF_TX_OK;
	rtl8xxxu_write8(priv, REG_DUAL_TSF_RST, val8);

exit:
	return ret;
}