void rtl8xxxu_restore_mac_regs(struct rtl8xxxu_priv *priv,
			       const u32 *reg, u32 *backup)
{
	int i;

	for (i = 0; i < (RTL8XXXU_MAC_REGS - 1); i++)
		rtl8xxxu_write8(priv, reg[i], backup[i]);

	rtl8xxxu_write32(priv, reg[i], backup[i]);
}