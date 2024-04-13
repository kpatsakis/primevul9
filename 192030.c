rtl8xxxu_init_mac(struct rtl8xxxu_priv *priv)
{
	struct rtl8xxxu_reg8val *array = priv->fops->mactable;
	int i, ret;
	u16 reg;
	u8 val;

	for (i = 0; ; i++) {
		reg = array[i].reg;
		val = array[i].val;

		if (reg == 0xffff && val == 0xff)
			break;

		ret = rtl8xxxu_write8(priv, reg, val);
		if (ret != 1) {
			dev_warn(&priv->udev->dev,
				 "Failed to initialize MAC "
				 "(reg: %04x, val %02x)\n", reg, val);
			return -EAGAIN;
		}
	}

	if (priv->rtl_chip != RTL8723B && priv->rtl_chip != RTL8192E)
		rtl8xxxu_write8(priv, REG_MAX_AGGR_NUM, 0x0a);

	return 0;
}