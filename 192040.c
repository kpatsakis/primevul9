u32 rtl8xxxu_read_rfreg(struct rtl8xxxu_priv *priv,
			enum rtl8xxxu_rfpath path, u8 reg)
{
	u32 hssia, val32, retval;

	hssia = rtl8xxxu_read32(priv, REG_FPGA0_XA_HSSI_PARM2);
	if (path != RF_A)
		val32 = rtl8xxxu_read32(priv, rtl8xxxu_rfregs[path].hssiparm2);
	else
		val32 = hssia;

	val32 &= ~FPGA0_HSSI_PARM2_ADDR_MASK;
	val32 |= (reg << FPGA0_HSSI_PARM2_ADDR_SHIFT);
	val32 |= FPGA0_HSSI_PARM2_EDGE_READ;
	hssia &= ~FPGA0_HSSI_PARM2_EDGE_READ;
	rtl8xxxu_write32(priv, REG_FPGA0_XA_HSSI_PARM2, hssia);

	udelay(10);

	rtl8xxxu_write32(priv, rtl8xxxu_rfregs[path].hssiparm2, val32);
	udelay(100);

	hssia |= FPGA0_HSSI_PARM2_EDGE_READ;
	rtl8xxxu_write32(priv, REG_FPGA0_XA_HSSI_PARM2, hssia);
	udelay(10);

	val32 = rtl8xxxu_read32(priv, rtl8xxxu_rfregs[path].hssiparm1);
	if (val32 & FPGA0_HSSI_PARM1_PI)
		retval = rtl8xxxu_read32(priv, rtl8xxxu_rfregs[path].hspiread);
	else
		retval = rtl8xxxu_read32(priv, rtl8xxxu_rfregs[path].lssiread);

	retval &= 0xfffff;

	if (rtl8xxxu_debug & RTL8XXXU_DEBUG_RFREG_READ)
		dev_info(&priv->udev->dev, "%s(%02x) = 0x%06x\n",
			 __func__, reg, retval);
	return retval;
}