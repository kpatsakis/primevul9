void rtl8xxxu_fill_iqk_matrix_a(struct rtl8xxxu_priv *priv, bool iqk_ok,
				int result[][8], int candidate, bool tx_only)
{
	u32 oldval, x, tx0_a, reg;
	int y, tx0_c;
	u32 val32;

	if (!iqk_ok)
		return;

	val32 = rtl8xxxu_read32(priv, REG_OFDM0_XA_TX_IQ_IMBALANCE);
	oldval = val32 >> 22;

	x = result[candidate][0];
	if ((x & 0x00000200) != 0)
		x = x | 0xfffffc00;
	tx0_a = (x * oldval) >> 8;

	val32 = rtl8xxxu_read32(priv, REG_OFDM0_XA_TX_IQ_IMBALANCE);
	val32 &= ~0x3ff;
	val32 |= tx0_a;
	rtl8xxxu_write32(priv, REG_OFDM0_XA_TX_IQ_IMBALANCE, val32);

	val32 = rtl8xxxu_read32(priv, REG_OFDM0_ENERGY_CCA_THRES);
	val32 &= ~BIT(31);
	if ((x * oldval >> 7) & 0x1)
		val32 |= BIT(31);
	rtl8xxxu_write32(priv, REG_OFDM0_ENERGY_CCA_THRES, val32);

	y = result[candidate][1];
	if ((y & 0x00000200) != 0)
		y = y | 0xfffffc00;
	tx0_c = (y * oldval) >> 8;

	val32 = rtl8xxxu_read32(priv, REG_OFDM0_XC_TX_AFE);
	val32 &= ~0xf0000000;
	val32 |= (((tx0_c & 0x3c0) >> 6) << 28);
	rtl8xxxu_write32(priv, REG_OFDM0_XC_TX_AFE, val32);

	val32 = rtl8xxxu_read32(priv, REG_OFDM0_XA_TX_IQ_IMBALANCE);
	val32 &= ~0x003f0000;
	val32 |= ((tx0_c & 0x3f) << 16);
	rtl8xxxu_write32(priv, REG_OFDM0_XA_TX_IQ_IMBALANCE, val32);

	val32 = rtl8xxxu_read32(priv, REG_OFDM0_ENERGY_CCA_THRES);
	val32 &= ~BIT(29);
	if ((y * oldval >> 7) & 0x1)
		val32 |= BIT(29);
	rtl8xxxu_write32(priv, REG_OFDM0_ENERGY_CCA_THRES, val32);

	if (tx_only) {
		dev_dbg(&priv->udev->dev, "%s: only TX\n", __func__);
		return;
	}

	reg = result[candidate][2];

	val32 = rtl8xxxu_read32(priv, REG_OFDM0_XA_RX_IQ_IMBALANCE);
	val32 &= ~0x3ff;
	val32 |= (reg & 0x3ff);
	rtl8xxxu_write32(priv, REG_OFDM0_XA_RX_IQ_IMBALANCE, val32);

	reg = result[candidate][3] & 0x3F;

	val32 = rtl8xxxu_read32(priv, REG_OFDM0_XA_RX_IQ_IMBALANCE);
	val32 &= ~0xfc00;
	val32 |= ((reg << 10) & 0xfc00);
	rtl8xxxu_write32(priv, REG_OFDM0_XA_RX_IQ_IMBALANCE, val32);

	reg = (result[candidate][3] >> 6) & 0xF;

	val32 = rtl8xxxu_read32(priv, REG_OFDM0_RX_IQ_EXT_ANTA);
	val32 &= ~0xf0000000;
	val32 |= (reg << 28);
	rtl8xxxu_write32(priv, REG_OFDM0_RX_IQ_EXT_ANTA, val32);
}