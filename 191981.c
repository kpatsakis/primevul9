int rtl8xxxu_init_phy_rf(struct rtl8xxxu_priv *priv,
			 struct rtl8xxxu_rfregval *table,
			 enum rtl8xxxu_rfpath path)
{
	u32 val32;
	u16 val16, rfsi_rfenv;
	u16 reg_sw_ctrl, reg_int_oe, reg_hssi_parm2;

	switch (path) {
	case RF_A:
		reg_sw_ctrl = REG_FPGA0_XA_RF_SW_CTRL;
		reg_int_oe = REG_FPGA0_XA_RF_INT_OE;
		reg_hssi_parm2 = REG_FPGA0_XA_HSSI_PARM2;
		break;
	case RF_B:
		reg_sw_ctrl = REG_FPGA0_XB_RF_SW_CTRL;
		reg_int_oe = REG_FPGA0_XB_RF_INT_OE;
		reg_hssi_parm2 = REG_FPGA0_XB_HSSI_PARM2;
		break;
	default:
		dev_err(&priv->udev->dev, "%s:Unsupported RF path %c\n",
			__func__, path + 'A');
		return -EINVAL;
	}
	/* For path B, use XB */
	rfsi_rfenv = rtl8xxxu_read16(priv, reg_sw_ctrl);
	rfsi_rfenv &= FPGA0_RF_RFENV;

	/*
	 * These two we might be able to optimize into one
	 */
	val32 = rtl8xxxu_read32(priv, reg_int_oe);
	val32 |= BIT(20);	/* 0x10 << 16 */
	rtl8xxxu_write32(priv, reg_int_oe, val32);
	udelay(1);

	val32 = rtl8xxxu_read32(priv, reg_int_oe);
	val32 |= BIT(4);
	rtl8xxxu_write32(priv, reg_int_oe, val32);
	udelay(1);

	/*
	 * These two we might be able to optimize into one
	 */
	val32 = rtl8xxxu_read32(priv, reg_hssi_parm2);
	val32 &= ~FPGA0_HSSI_3WIRE_ADDR_LEN;
	rtl8xxxu_write32(priv, reg_hssi_parm2, val32);
	udelay(1);

	val32 = rtl8xxxu_read32(priv, reg_hssi_parm2);
	val32 &= ~FPGA0_HSSI_3WIRE_DATA_LEN;
	rtl8xxxu_write32(priv, reg_hssi_parm2, val32);
	udelay(1);

	rtl8xxxu_init_rf_regs(priv, table, path);

	/* For path B, use XB */
	val16 = rtl8xxxu_read16(priv, reg_sw_ctrl);
	val16 &= ~FPGA0_RF_RFENV;
	val16 |= rfsi_rfenv;
	rtl8xxxu_write16(priv, reg_sw_ctrl, val16);

	return 0;
}