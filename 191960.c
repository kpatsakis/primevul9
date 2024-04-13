static int rtl8xxxu_download_firmware(struct rtl8xxxu_priv *priv)
{
	int pages, remainder, i, ret;
	u8 val8;
	u16 val16;
	u32 val32;
	u8 *fwptr;

	val8 = rtl8xxxu_read8(priv, REG_SYS_FUNC + 1);
	val8 |= 4;
	rtl8xxxu_write8(priv, REG_SYS_FUNC + 1, val8);

	/* 8051 enable */
	val16 = rtl8xxxu_read16(priv, REG_SYS_FUNC);
	val16 |= SYS_FUNC_CPU_ENABLE;
	rtl8xxxu_write16(priv, REG_SYS_FUNC, val16);

	val8 = rtl8xxxu_read8(priv, REG_MCU_FW_DL);
	if (val8 & MCU_FW_RAM_SEL) {
		pr_info("do the RAM reset\n");
		rtl8xxxu_write8(priv, REG_MCU_FW_DL, 0x00);
		priv->fops->reset_8051(priv);
	}

	/* MCU firmware download enable */
	val8 = rtl8xxxu_read8(priv, REG_MCU_FW_DL);
	val8 |= MCU_FW_DL_ENABLE;
	rtl8xxxu_write8(priv, REG_MCU_FW_DL, val8);

	/* 8051 reset */
	val32 = rtl8xxxu_read32(priv, REG_MCU_FW_DL);
	val32 &= ~BIT(19);
	rtl8xxxu_write32(priv, REG_MCU_FW_DL, val32);

	/* Reset firmware download checksum */
	val8 = rtl8xxxu_read8(priv, REG_MCU_FW_DL);
	val8 |= MCU_FW_DL_CSUM_REPORT;
	rtl8xxxu_write8(priv, REG_MCU_FW_DL, val8);

	pages = priv->fw_size / RTL_FW_PAGE_SIZE;
	remainder = priv->fw_size % RTL_FW_PAGE_SIZE;

	fwptr = priv->fw_data->data;

	for (i = 0; i < pages; i++) {
		val8 = rtl8xxxu_read8(priv, REG_MCU_FW_DL + 2) & 0xF8;
		val8 |= i;
		rtl8xxxu_write8(priv, REG_MCU_FW_DL + 2, val8);

		ret = rtl8xxxu_writeN(priv, REG_FW_START_ADDRESS,
				      fwptr, RTL_FW_PAGE_SIZE);
		if (ret != RTL_FW_PAGE_SIZE) {
			ret = -EAGAIN;
			goto fw_abort;
		}

		fwptr += RTL_FW_PAGE_SIZE;
	}

	if (remainder) {
		val8 = rtl8xxxu_read8(priv, REG_MCU_FW_DL + 2) & 0xF8;
		val8 |= i;
		rtl8xxxu_write8(priv, REG_MCU_FW_DL + 2, val8);
		ret = rtl8xxxu_writeN(priv, REG_FW_START_ADDRESS,
				      fwptr, remainder);
		if (ret != remainder) {
			ret = -EAGAIN;
			goto fw_abort;
		}
	}

	ret = 0;
fw_abort:
	/* MCU firmware download disable */
	val16 = rtl8xxxu_read16(priv, REG_MCU_FW_DL);
	val16 &= ~MCU_FW_DL_ENABLE;
	rtl8xxxu_write16(priv, REG_MCU_FW_DL, val16);

	return ret;
}