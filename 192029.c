static int rtl8xxxu_init_rf_regs(struct rtl8xxxu_priv *priv,
				 struct rtl8xxxu_rfregval *array,
				 enum rtl8xxxu_rfpath path)
{
	int i, ret;
	u8 reg;
	u32 val;

	for (i = 0; ; i++) {
		reg = array[i].reg;
		val = array[i].val;

		if (reg == 0xff && val == 0xffffffff)
			break;

		switch (reg) {
		case 0xfe:
			msleep(50);
			continue;
		case 0xfd:
			mdelay(5);
			continue;
		case 0xfc:
			mdelay(1);
			continue;
		case 0xfb:
			udelay(50);
			continue;
		case 0xfa:
			udelay(5);
			continue;
		case 0xf9:
			udelay(1);
			continue;
		}

		ret = rtl8xxxu_write_rfreg(priv, path, reg, val);
		if (ret) {
			dev_warn(&priv->udev->dev,
				 "Failed to initialize RF\n");
			return -EAGAIN;
		}
		udelay(1);
	}

	return 0;
}