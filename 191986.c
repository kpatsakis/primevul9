void rtl8xxxu_fill_iqk_matrix_b(struct rtl8xxxu_priv *priv, bool iqk_ok,
				int result[][8], int candidate, bool tx_only)
{
	u32 oldval, x, tx1_a, reg;
	int y, tx1_c;
	u32 val32;

	if (!iqk_ok)
		return;

	val32 = rtl8xxxu_read32(priv, REG_OFDM0_XB_TX_IQ_IMBALANCE);
	oldval = val32 >> 22;

	x = result[candidate][4];
	if ((x & 0x00000200) != 0)
		x = x | 0xfffffc00;
	tx1_a = (x * oldval) >> 8;

	val32 = rtl8xxxu_read32(priv, REG_OFDM0_XB_TX_IQ_IMBALANCE);
	val32 &= ~0x3ff;
	val32 |= tx1_a;
	rtl8xxxu_write32(priv, REG_OFDM0_XB_TX_IQ_IMBALANCE, val32);

	val32 = rtl8xxxu_read32(priv, REG_OFDM0_ENERGY_CCA_THRES);
	val32 &= ~BIT(27);
	if ((x * oldval >> 7) & 0x1)
		val32 |= BIT(27);
	rtl8xxxu_write32(priv, REG_OFDM0_ENERGY_CCA_THRES, val32);

	y = result[candidate][5];
	if ((y & 0x00000200) != 0)
		y = y | 0xfffffc00;
	tx1_c = (y * oldval) >> 8;

	val32 = rtl8xxxu_read32(priv, REG_OFDM0_XD_TX_AFE);
	val32 &= ~0xf0000000;
	val32 |= (((tx1_c & 0x3c0) >> 6) << 28);
	rtl8xxxu_write32(priv, REG_OFDM0_XD_TX_AFE, val32);

	val32 = rtl8xxxu_read32(priv, REG_OFDM0_XB_TX_IQ_IMBALANCE);
	val32 &= ~0x003f0000;
	val32 |= ((tx1_c & 0x3f) << 16);
	rtl8xxxu_write32(priv, REG_OFDM0_XB_TX_IQ_IMBALANCE, val32);

	val32 = rtl8xxxu_read32(priv, REG_OFDM0_ENERGY_CCA_THRES);
	val32 &= ~BIT(25);
	if ((y * oldval >> 7) & 0x1)
		val32 |= BIT(25);
	rtl8xxxu_write32(priv, REG_OFDM0_ENERGY_CCA_THRES, val32);

	if (tx_only) {
		dev_dbg(&priv->udev->dev, "%s: only TX\n", __func__);
		return;
	}

	reg = result[candidate][6];

	val32 = rtl8xxxu_read32(priv, REG_OFDM0_XB_RX_IQ_IMBALANCE);
	val32 &= ~0x3ff;
	val32 |= (reg & 0x3ff);
	rtl8xxxu_write32(priv, REG_OFDM0_XB_RX_IQ_IMBALANCE, val32);

	reg = result[candidate][7] & 0x3f;

	val32 = rtl8xxxu_read32(priv, REG_OFDM0_XB_RX_IQ_IMBALANCE);
	val32 &= ~0xfc00;
	val32 |= ((reg << 10) & 0xfc00);
	rtl8xxxu_write32(priv, REG_OFDM0_XB_RX_IQ_IMBALANCE, val32);

	reg = (result[candidate][7] >> 6) & 0xf;

	val32 = rtl8xxxu_read32(priv, REG_OFDM0_AGCR_SSI_TABLE);
	val32 &= ~0x0000f000;
	val32 |= (reg << 12);
	rtl8xxxu_write32(priv, REG_OFDM0_AGCR_SSI_TABLE, val32);
}