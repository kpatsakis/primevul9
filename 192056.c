void rtl8xxxu_gen1_init_phy_bb(struct rtl8xxxu_priv *priv)
{
	u8 val8, ldoa15, ldov12d, lpldo, ldohci12;
	u16 val16;
	u32 val32;

	val8 = rtl8xxxu_read8(priv, REG_AFE_PLL_CTRL);
	udelay(2);
	val8 |= AFE_PLL_320_ENABLE;
	rtl8xxxu_write8(priv, REG_AFE_PLL_CTRL, val8);
	udelay(2);

	rtl8xxxu_write8(priv, REG_AFE_PLL_CTRL + 1, 0xff);
	udelay(2);

	val16 = rtl8xxxu_read16(priv, REG_SYS_FUNC);
	val16 |= SYS_FUNC_BB_GLB_RSTN | SYS_FUNC_BBRSTB;
	rtl8xxxu_write16(priv, REG_SYS_FUNC, val16);

	val32 = rtl8xxxu_read32(priv, REG_AFE_XTAL_CTRL);
	val32 &= ~AFE_XTAL_RF_GATE;
	if (priv->has_bluetooth)
		val32 &= ~AFE_XTAL_BT_GATE;
	rtl8xxxu_write32(priv, REG_AFE_XTAL_CTRL, val32);

	/* 6. 0x1f[7:0] = 0x07 */
	val8 = RF_ENABLE | RF_RSTB | RF_SDMRSTB;
	rtl8xxxu_write8(priv, REG_RF_CTRL, val8);

	if (priv->hi_pa)
		rtl8xxxu_init_phy_regs(priv, rtl8188ru_phy_1t_highpa_table);
	else if (priv->tx_paths == 2)
		rtl8xxxu_init_phy_regs(priv, rtl8192cu_phy_2t_init_table);
	else
		rtl8xxxu_init_phy_regs(priv, rtl8723a_phy_1t_init_table);

	if (priv->rtl_chip == RTL8188R && priv->hi_pa &&
	    priv->vendor_umc && priv->chip_cut == 1)
		rtl8xxxu_write8(priv, REG_OFDM0_AGC_PARM1 + 2, 0x50);

	if (priv->hi_pa)
		rtl8xxxu_init_phy_regs(priv, rtl8xxx_agc_highpa_table);
	else
		rtl8xxxu_init_phy_regs(priv, rtl8xxx_agc_standard_table);

	ldoa15 = LDOA15_ENABLE | LDOA15_OBUF;
	ldov12d = LDOV12D_ENABLE | BIT(2) | (2 << LDOV12D_VADJ_SHIFT);
	ldohci12 = 0x57;
	lpldo = 1;
	val32 = (lpldo << 24) | (ldohci12 << 16) | (ldov12d << 8) | ldoa15;
	rtl8xxxu_write32(priv, REG_LDOA15_CTRL, val32);
}