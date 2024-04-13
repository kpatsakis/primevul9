void rtl8xxxu_save_regs(struct rtl8xxxu_priv *priv, const u32 *regs,
			u32 *backup, int count)
{
	int i;

	for (i = 0; i < count; i++)
		backup[i] = rtl8xxxu_read32(priv, regs[i]);
}