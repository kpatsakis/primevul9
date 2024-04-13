static int rtl8xxxu_emu_to_disabled(struct rtl8xxxu_priv *priv)
{
	u8 val8;

	/* 0x0007[7:0] = 0x20 SOP option to disable BG/MB */
	rtl8xxxu_write8(priv, REG_APS_FSMCO + 3, 0x20);

	/* 0x04[12:11] = 01 enable WL suspend */
	val8 = rtl8xxxu_read8(priv, REG_APS_FSMCO + 1);
	val8 &= ~BIT(4);
	val8 |= BIT(3);
	rtl8xxxu_write8(priv, REG_APS_FSMCO + 1, val8);

	val8 = rtl8xxxu_read8(priv, REG_APS_FSMCO + 1);
	val8 |= BIT(7);
	rtl8xxxu_write8(priv, REG_APS_FSMCO + 1, val8);

	/* 0x48[16] = 1 to enable GPIO9 as EXT wakeup */
	val8 = rtl8xxxu_read8(priv, REG_GPIO_INTM + 2);
	val8 |= BIT(0);
	rtl8xxxu_write8(priv, REG_GPIO_INTM + 2, val8);

	return 0;
}