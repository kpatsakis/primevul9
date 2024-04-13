void rtl8xxxu_mac_calibration(struct rtl8xxxu_priv *priv,
			      const u32 *regs, u32 *backup)
{
	int i = 0;

	rtl8xxxu_write8(priv, regs[i], 0x3f);

	for (i = 1 ; i < (RTL8XXXU_MAC_REGS - 1); i++)
		rtl8xxxu_write8(priv, regs[i], (u8)(backup[i] & ~BIT(3)));

	rtl8xxxu_write8(priv, regs[i], (u8)(backup[i] & ~BIT(5)));
}