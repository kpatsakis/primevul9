int rtl8xxxu_write_rfreg(struct rtl8xxxu_priv *priv,
			 enum rtl8xxxu_rfpath path, u8 reg, u32 data)
{
	int ret, retval;
	u32 dataaddr, val32;

	if (rtl8xxxu_debug & RTL8XXXU_DEBUG_RFREG_WRITE)
		dev_info(&priv->udev->dev, "%s(%02x) = 0x%06x\n",
			 __func__, reg, data);

	data &= FPGA0_LSSI_PARM_DATA_MASK;
	dataaddr = (reg << FPGA0_LSSI_PARM_ADDR_SHIFT) | data;

	if (priv->rtl_chip == RTL8192E) {
		val32 = rtl8xxxu_read32(priv, REG_FPGA0_POWER_SAVE);
		val32 &= ~0x20000;
		rtl8xxxu_write32(priv, REG_FPGA0_POWER_SAVE, val32);
	}

	/* Use XB for path B */
	ret = rtl8xxxu_write32(priv, rtl8xxxu_rfregs[path].lssiparm, dataaddr);
	if (ret != sizeof(dataaddr))
		retval = -EIO;
	else
		retval = 0;

	udelay(1);

	if (priv->rtl_chip == RTL8192E) {
		val32 = rtl8xxxu_read32(priv, REG_FPGA0_POWER_SAVE);
		val32 |= 0x20000;
		rtl8xxxu_write32(priv, REG_FPGA0_POWER_SAVE, val32);
	}

	return retval;
}