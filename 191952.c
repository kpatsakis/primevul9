static int rtl8xxxu_start_firmware(struct rtl8xxxu_priv *priv)
{
	struct device *dev = &priv->udev->dev;
	int ret = 0, i;
	u32 val32;

	/* Poll checksum report */
	for (i = 0; i < RTL8XXXU_FIRMWARE_POLL_MAX; i++) {
		val32 = rtl8xxxu_read32(priv, REG_MCU_FW_DL);
		if (val32 & MCU_FW_DL_CSUM_REPORT)
			break;
	}

	if (i == RTL8XXXU_FIRMWARE_POLL_MAX) {
		dev_warn(dev, "Firmware checksum poll timed out\n");
		ret = -EAGAIN;
		goto exit;
	}

	val32 = rtl8xxxu_read32(priv, REG_MCU_FW_DL);
	val32 |= MCU_FW_DL_READY;
	val32 &= ~MCU_WINT_INIT_READY;
	rtl8xxxu_write32(priv, REG_MCU_FW_DL, val32);

	/*
	 * Reset the 8051 in order for the firmware to start running,
	 * otherwise it won't come up on the 8192eu
	 */
	priv->fops->reset_8051(priv);

	/* Wait for firmware to become ready */
	for (i = 0; i < RTL8XXXU_FIRMWARE_POLL_MAX; i++) {
		val32 = rtl8xxxu_read32(priv, REG_MCU_FW_DL);
		if (val32 & MCU_WINT_INIT_READY)
			break;

		udelay(100);
	}

	if (i == RTL8XXXU_FIRMWARE_POLL_MAX) {
		dev_warn(dev, "Firmware failed to start\n");
		ret = -EAGAIN;
		goto exit;
	}

	/*
	 * Init H2C command
	 */
	if (priv->rtl_chip == RTL8723B)
		rtl8xxxu_write8(priv, REG_HMTFR, 0x0f);
exit:
	return ret;
}