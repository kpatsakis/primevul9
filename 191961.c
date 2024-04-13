rtl8xxxu_save_mac_regs(struct rtl8xxxu_priv *priv, const u32 *reg, u32 *backup)
{
	int i;

	for (i = 0; i < (RTL8XXXU_MAC_REGS - 1); i++)
		backup[i] = rtl8xxxu_read8(priv, reg[i]);

	backup[i] = rtl8xxxu_read32(priv, reg[i]);
}