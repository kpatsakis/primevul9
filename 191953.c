static int rtl8xxxu_iqk_path_a(struct rtl8xxxu_priv *priv)
{
	u32 reg_eac, reg_e94, reg_e9c, reg_ea4, val32;
	int result = 0;

	/* path-A IQK setting */
	rtl8xxxu_write32(priv, REG_TX_IQK_TONE_A, 0x10008c1f);
	rtl8xxxu_write32(priv, REG_RX_IQK_TONE_A, 0x10008c1f);
	rtl8xxxu_write32(priv, REG_TX_IQK_PI_A, 0x82140102);

	val32 = (priv->rf_paths > 1) ? 0x28160202 :
		/*IS_81xxC_VENDOR_UMC_B_CUT(pHalData->VersionID)?0x28160202: */
		0x28160502;
	rtl8xxxu_write32(priv, REG_RX_IQK_PI_A, val32);

	/* path-B IQK setting */
	if (priv->rf_paths > 1) {
		rtl8xxxu_write32(priv, REG_TX_IQK_TONE_B, 0x10008c22);
		rtl8xxxu_write32(priv, REG_RX_IQK_TONE_B, 0x10008c22);
		rtl8xxxu_write32(priv, REG_TX_IQK_PI_B, 0x82140102);
		rtl8xxxu_write32(priv, REG_RX_IQK_PI_B, 0x28160202);
	}

	/* LO calibration setting */
	rtl8xxxu_write32(priv, REG_IQK_AGC_RSP, 0x001028d1);

	/* One shot, path A LOK & IQK */
	rtl8xxxu_write32(priv, REG_IQK_AGC_PTS, 0xf9000000);
	rtl8xxxu_write32(priv, REG_IQK_AGC_PTS, 0xf8000000);

	mdelay(1);

	/* Check failed */
	reg_eac = rtl8xxxu_read32(priv, REG_RX_POWER_AFTER_IQK_A_2);
	reg_e94 = rtl8xxxu_read32(priv, REG_TX_POWER_BEFORE_IQK_A);
	reg_e9c = rtl8xxxu_read32(priv, REG_TX_POWER_AFTER_IQK_A);
	reg_ea4 = rtl8xxxu_read32(priv, REG_RX_POWER_BEFORE_IQK_A_2);

	if (!(reg_eac & BIT(28)) &&
	    ((reg_e94 & 0x03ff0000) != 0x01420000) &&
	    ((reg_e9c & 0x03ff0000) != 0x00420000))
		result |= 0x01;
	else	/* If TX not OK, ignore RX */
		goto out;

	/* If TX is OK, check whether RX is OK */
	if (!(reg_eac & BIT(27)) &&
	    ((reg_ea4 & 0x03ff0000) != 0x01320000) &&
	    ((reg_eac & 0x03ff0000) != 0x00360000))
		result |= 0x02;
	else
		dev_warn(&priv->udev->dev, "%s: Path A RX IQK failed!\n",
			 __func__);
out:
	return result;
}