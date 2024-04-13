void rtl8xxxu_gen1_disable_rf(struct rtl8xxxu_priv *priv)
{
	u8 sps0;
	u32 val32;

	sps0 = rtl8xxxu_read8(priv, REG_SPS0_CTRL);

	/* RF RX code for preamble power saving */
	val32 = rtl8xxxu_read32(priv, REG_FPGA0_XAB_RF_PARM);
	val32 &= ~(BIT(3) | BIT(4) | BIT(5));
	if (priv->rf_paths == 2)
		val32 &= ~(BIT(19) | BIT(20) | BIT(21));
	rtl8xxxu_write32(priv, REG_FPGA0_XAB_RF_PARM, val32);

	/* Disable TX for four paths */
	val32 = rtl8xxxu_read32(priv, REG_OFDM0_TRX_PATH_ENABLE);
	val32 &= ~OFDM_RF_PATH_TX_MASK;
	rtl8xxxu_write32(priv, REG_OFDM0_TRX_PATH_ENABLE, val32);

	/* Enable power saving */
	val32 = rtl8xxxu_read32(priv, REG_FPGA0_RF_MODE);
	val32 |= FPGA_RF_MODE_JAPAN;
	rtl8xxxu_write32(priv, REG_FPGA0_RF_MODE, val32);

	/* AFE control register to power down bits [30:22] */
	if (priv->rf_paths == 2)
		rtl8xxxu_write32(priv, REG_RX_WAIT_CCA, 0x00db25a0);
	else
		rtl8xxxu_write32(priv, REG_RX_WAIT_CCA, 0x001b25a0);

	/* Power down RF module */
	rtl8xxxu_write_rfreg(priv, RF_A, RF6052_REG_AC, 0);
	if (priv->rf_paths == 2)
		rtl8xxxu_write_rfreg(priv, RF_B, RF6052_REG_AC, 0);

	sps0 &= ~(BIT(0) | BIT(3));
	rtl8xxxu_write8(priv, REG_SPS0_CTRL, sps0);
}