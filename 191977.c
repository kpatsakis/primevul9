void rtl8xxxu_restore_regs(struct rtl8xxxu_priv *priv, const u32 *regs,
			   u32 *backup, int count)
{
	int i;

	for (i = 0; i < count; i++)
		rtl8xxxu_write32(priv, regs[i], backup[i]);
}