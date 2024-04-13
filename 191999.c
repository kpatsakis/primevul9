void rtl8xxxu_reset_8051(struct rtl8xxxu_priv *priv)
{
	u8 val8;
	u16 sys_func;

	val8 = rtl8xxxu_read8(priv, REG_RSV_CTRL + 1);
	val8 &= ~BIT(0);
	rtl8xxxu_write8(priv, REG_RSV_CTRL + 1, val8);

	sys_func = rtl8xxxu_read16(priv, REG_SYS_FUNC);
	sys_func &= ~SYS_FUNC_CPU_ENABLE;
	rtl8xxxu_write16(priv, REG_SYS_FUNC, sys_func);

	val8 = rtl8xxxu_read8(priv, REG_RSV_CTRL + 1);
	val8 |= BIT(0);
	rtl8xxxu_write8(priv, REG_RSV_CTRL + 1, val8);

	sys_func |= SYS_FUNC_CPU_ENABLE;
	rtl8xxxu_write16(priv, REG_SYS_FUNC, sys_func);
}