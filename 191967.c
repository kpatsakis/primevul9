void rtl8xxxu_gen1_enable_rf(struct rtl8xxxu_priv *priv)
{
	u8 val8;
	u32 val32;

	val8 = rtl8xxxu_read8(priv, REG_SPS0_CTRL);
	val8 |= BIT(0) | BIT(3);
	rtl8xxxu_write8(priv, REG_SPS0_CTRL, val8);

	val32 = rtl8xxxu_read32(priv, REG_FPGA0_XAB_RF_PARM);
	val32 &= ~(BIT(4) | BIT(5));
	val32 |= BIT(3);
	if (priv->rf_paths == 2) {
		val32 &= ~(BIT(20) | BIT(21));
		val32 |= BIT(19);
	}
	rtl8xxxu_write32(priv, REG_FPGA0_XAB_RF_PARM, val32);

	val32 = rtl8xxxu_read32(priv, REG_OFDM0_TRX_PATH_ENABLE);
	val32 &= ~OFDM_RF_PATH_TX_MASK;
	if (priv->tx_paths == 2)
		val32 |= OFDM_RF_PATH_TX_A | OFDM_RF_PATH_TX_B;
	else if (priv->rtl_chip == RTL8192C || priv->rtl_chip == RTL8191C)
		val32 |= OFDM_RF_PATH_TX_B;
	else
		val32 |= OFDM_RF_PATH_TX_A;
	rtl8xxxu_write32(priv, REG_OFDM0_TRX_PATH_ENABLE, val32);

	val32 = rtl8xxxu_read32(priv, REG_FPGA0_RF_MODE);
	val32 &= ~FPGA_RF_MODE_JAPAN;
	rtl8xxxu_write32(priv, REG_FPGA0_RF_MODE, val32);

	if (priv->rf_paths == 2)
		rtl8xxxu_write32(priv, REG_RX_WAIT_CCA, 0x63db25a0);
	else
		rtl8xxxu_write32(priv, REG_RX_WAIT_CCA, 0x631b25a0);

	rtl8xxxu_write_rfreg(priv, RF_A, RF6052_REG_AC, 0x32d95);
	if (priv->rf_paths == 2)
		rtl8xxxu_write_rfreg(priv, RF_B, RF6052_REG_AC, 0x32d95);

	rtl8xxxu_write8(priv, REG_TXPAUSE, 0x00);
}