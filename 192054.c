static int rtl8xxxu_read_efuse(struct rtl8xxxu_priv *priv)
{
	struct device *dev = &priv->udev->dev;
	int i, ret = 0;
	u8 val8, word_mask, header, extheader;
	u16 val16, efuse_addr, offset;
	u32 val32;

	val16 = rtl8xxxu_read16(priv, REG_9346CR);
	if (val16 & EEPROM_ENABLE)
		priv->has_eeprom = 1;
	if (val16 & EEPROM_BOOT)
		priv->boot_eeprom = 1;

	if (priv->is_multi_func) {
		val32 = rtl8xxxu_read32(priv, REG_EFUSE_TEST);
		val32 = (val32 & ~EFUSE_SELECT_MASK) | EFUSE_WIFI_SELECT;
		rtl8xxxu_write32(priv, REG_EFUSE_TEST, val32);
	}

	dev_dbg(dev, "Booting from %s\n",
		priv->boot_eeprom ? "EEPROM" : "EFUSE");

	rtl8xxxu_write8(priv, REG_EFUSE_ACCESS, EFUSE_ACCESS_ENABLE);

	/*  1.2V Power: From VDDON with Power Cut(0x0000[15]), default valid */
	val16 = rtl8xxxu_read16(priv, REG_SYS_ISO_CTRL);
	if (!(val16 & SYS_ISO_PWC_EV12V)) {
		val16 |= SYS_ISO_PWC_EV12V;
		rtl8xxxu_write16(priv, REG_SYS_ISO_CTRL, val16);
	}
	/*  Reset: 0x0000[28], default valid */
	val16 = rtl8xxxu_read16(priv, REG_SYS_FUNC);
	if (!(val16 & SYS_FUNC_ELDR)) {
		val16 |= SYS_FUNC_ELDR;
		rtl8xxxu_write16(priv, REG_SYS_FUNC, val16);
	}

	/*
	 * Clock: Gated(0x0008[5]) 8M(0x0008[1]) clock from ANA, default valid
	 */
	val16 = rtl8xxxu_read16(priv, REG_SYS_CLKR);
	if (!(val16 & SYS_CLK_LOADER_ENABLE) || !(val16 & SYS_CLK_ANA8M)) {
		val16 |= (SYS_CLK_LOADER_ENABLE | SYS_CLK_ANA8M);
		rtl8xxxu_write16(priv, REG_SYS_CLKR, val16);
	}

	/* Default value is 0xff */
	memset(priv->efuse_wifi.raw, 0xff, EFUSE_MAP_LEN);

	efuse_addr = 0;
	while (efuse_addr < EFUSE_REAL_CONTENT_LEN_8723A) {
		u16 map_addr;

		ret = rtl8xxxu_read_efuse8(priv, efuse_addr++, &header);
		if (ret || header == 0xff)
			goto exit;

		if ((header & 0x1f) == 0x0f) {	/* extended header */
			offset = (header & 0xe0) >> 5;

			ret = rtl8xxxu_read_efuse8(priv, efuse_addr++,
						   &extheader);
			if (ret)
				goto exit;
			/* All words disabled */
			if ((extheader & 0x0f) == 0x0f)
				continue;

			offset |= ((extheader & 0xf0) >> 1);
			word_mask = extheader & 0x0f;
		} else {
			offset = (header >> 4) & 0x0f;
			word_mask = header & 0x0f;
		}

		/* Get word enable value from PG header */

		/* We have 8 bits to indicate validity */
		map_addr = offset * 8;
		if (map_addr >= EFUSE_MAP_LEN) {
			dev_warn(dev, "%s: Illegal map_addr (%04x), "
				 "efuse corrupt!\n",
				 __func__, map_addr);
			ret = -EINVAL;
			goto exit;
		}
		for (i = 0; i < EFUSE_MAX_WORD_UNIT; i++) {
			/* Check word enable condition in the section */
			if (word_mask & BIT(i)) {
				map_addr += 2;
				continue;
			}

			ret = rtl8xxxu_read_efuse8(priv, efuse_addr++, &val8);
			if (ret)
				goto exit;
			priv->efuse_wifi.raw[map_addr++] = val8;

			ret = rtl8xxxu_read_efuse8(priv, efuse_addr++, &val8);
			if (ret)
				goto exit;
			priv->efuse_wifi.raw[map_addr++] = val8;
		}
	}

exit:
	rtl8xxxu_write8(priv, REG_EFUSE_ACCESS, EFUSE_ACCESS_DISABLE);

	return ret;
}