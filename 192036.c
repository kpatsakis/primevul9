rtl8xxxu_read_efuse8(struct rtl8xxxu_priv *priv, u16 offset, u8 *data)
{
	int i;
	u8 val8;
	u32 val32;

	/* Write Address */
	rtl8xxxu_write8(priv, REG_EFUSE_CTRL + 1, offset & 0xff);
	val8 = rtl8xxxu_read8(priv, REG_EFUSE_CTRL + 2);
	val8 &= 0xfc;
	val8 |= (offset >> 8) & 0x03;
	rtl8xxxu_write8(priv, REG_EFUSE_CTRL + 2, val8);

	val8 = rtl8xxxu_read8(priv, REG_EFUSE_CTRL + 3);
	rtl8xxxu_write8(priv, REG_EFUSE_CTRL + 3, val8 & 0x7f);

	/* Poll for data read */
	val32 = rtl8xxxu_read32(priv, REG_EFUSE_CTRL);
	for (i = 0; i < RTL8XXXU_MAX_REG_POLL; i++) {
		val32 = rtl8xxxu_read32(priv, REG_EFUSE_CTRL);
		if (val32 & BIT(31))
			break;
	}

	if (i == RTL8XXXU_MAX_REG_POLL)
		return -EIO;

	udelay(50);
	val32 = rtl8xxxu_read32(priv, REG_EFUSE_CTRL);

	*data = val32 & 0xff;
	return 0;
}