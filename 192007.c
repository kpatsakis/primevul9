static void rtl8xxxu_set_ampdu_min_space(struct rtl8xxxu_priv *priv, u8 density)
{
	u8 val8;

	val8 = rtl8xxxu_read8(priv, REG_AMPDU_MIN_SPACE);
	val8 &= 0xf8;
	val8 |= density;
	rtl8xxxu_write8(priv, REG_AMPDU_MIN_SPACE, val8);
}