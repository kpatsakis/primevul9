static int rtl8xxxu_iqk_path_b(struct rtl8xxxu_priv *priv)
{
	u32 reg_eac, reg_eb4, reg_ebc, reg_ec4, reg_ecc;
	int result = 0;

	/* One shot, path B LOK & IQK */
	rtl8xxxu_write32(priv, REG_IQK_AGC_CONT, 0x00000002);
	rtl8xxxu_write32(priv, REG_IQK_AGC_CONT, 0x00000000);

	mdelay(1);

	/* Check failed */
	reg_eac = rtl8xxxu_read32(priv, REG_RX_POWER_AFTER_IQK_A_2);
	reg_eb4 = rtl8xxxu_read32(priv, REG_TX_POWER_BEFORE_IQK_B);
	reg_ebc = rtl8xxxu_read32(priv, REG_TX_POWER_AFTER_IQK_B);
	reg_ec4 = rtl8xxxu_read32(priv, REG_RX_POWER_BEFORE_IQK_B_2);
	reg_ecc = rtl8xxxu_read32(priv, REG_RX_POWER_AFTER_IQK_B_2);

	if (!(reg_eac & BIT(31)) &&
	    ((reg_eb4 & 0x03ff0000) != 0x01420000) &&
	    ((reg_ebc & 0x03ff0000) != 0x00420000))
		result |= 0x01;
	else
		goto out;

	if (!(reg_eac & BIT(30)) &&
	    (((reg_ec4 & 0x03ff0000) >> 16) != 0x132) &&
	    (((reg_ecc & 0x03ff0000) >> 16) != 0x36))
		result |= 0x02;
	else
		dev_warn(&priv->udev->dev, "%s: Path B RX IQK failed!\n",
			 __func__);
out:
	return result;
}