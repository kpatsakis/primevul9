static void rtl8xxxu_stop_tx_beacon(struct rtl8xxxu_priv *priv)
{
	u8 val8;

	val8 = rtl8xxxu_read8(priv, REG_FWHW_TXQ_CTRL + 2);
	val8 &= ~BIT(6);
	rtl8xxxu_write8(priv, REG_FWHW_TXQ_CTRL + 2, val8);

	rtl8xxxu_write8(priv, REG_TBTT_PROHIBIT + 1, 0x64);
	val8 = rtl8xxxu_read8(priv, REG_TBTT_PROHIBIT + 2);
	val8 &= ~BIT(0);
	rtl8xxxu_write8(priv, REG_TBTT_PROHIBIT + 2, val8);
}