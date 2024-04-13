void rtl8xxxu_firmware_self_reset(struct rtl8xxxu_priv *priv)
{
	u16 val16;
	int i = 100;

	/* Inform 8051 to perform reset */
	rtl8xxxu_write8(priv, REG_HMTFR + 3, 0x20);

	for (i = 100; i > 0; i--) {
		val16 = rtl8xxxu_read16(priv, REG_SYS_FUNC);

		if (!(val16 & SYS_FUNC_CPU_ENABLE)) {
			dev_dbg(&priv->udev->dev,
				"%s: Firmware self reset success!\n", __func__);
			break;
		}
		udelay(50);
	}

	if (!i) {
		/* Force firmware reset */
		val16 = rtl8xxxu_read16(priv, REG_SYS_FUNC);
		val16 &= ~SYS_FUNC_CPU_ENABLE;
		rtl8xxxu_write16(priv, REG_SYS_FUNC, val16);
	}
}