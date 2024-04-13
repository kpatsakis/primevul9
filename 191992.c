void rtl8xxxu_gen2_disable_rf(struct rtl8xxxu_priv *priv)
{
	u32 val32;

	val32 = rtl8xxxu_read32(priv, REG_RX_WAIT_CCA);
	val32 &= ~(BIT(22) | BIT(23));
	rtl8xxxu_write32(priv, REG_RX_WAIT_CCA, val32);
}