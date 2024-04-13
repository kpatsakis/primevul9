int rtl8xxxu_init_phy_regs(struct rtl8xxxu_priv *priv,
			   struct rtl8xxxu_reg32val *array)
{
	int i, ret;
	u16 reg;
	u32 val;

	for (i = 0; ; i++) {
		reg = array[i].reg;
		val = array[i].val;

		if (reg == 0xffff && val == 0xffffffff)
			break;

		ret = rtl8xxxu_write32(priv, reg, val);
		if (ret != sizeof(val)) {
			dev_warn(&priv->udev->dev,
				 "Failed to initialize PHY\n");
			return -EAGAIN;
		}
		udelay(1);
	}

	return 0;
}