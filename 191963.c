void rtl8xxxu_disabled_to_emu(struct rtl8xxxu_priv *priv)
{
	u8 val8;

	/* Clear suspend enable and power down enable*/
	val8 = rtl8xxxu_read8(priv, REG_APS_FSMCO + 1);
	val8 &= ~(BIT(3) | BIT(7));
	rtl8xxxu_write8(priv, REG_APS_FSMCO + 1, val8);

	/* 0x48[16] = 0 to disable GPIO9 as EXT WAKEUP*/
	val8 = rtl8xxxu_read8(priv, REG_GPIO_INTM + 2);
	val8 &= ~BIT(0);
	rtl8xxxu_write8(priv, REG_GPIO_INTM + 2, val8);

	/* 0x04[12:11] = 11 enable WL suspend*/
	val8 = rtl8xxxu_read8(priv, REG_APS_FSMCO + 1);
	val8 &= ~(BIT(3) | BIT(4));
	rtl8xxxu_write8(priv, REG_APS_FSMCO + 1, val8);
}