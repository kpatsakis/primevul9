static void rtl8xxxu_phy_iqcalibrate(struct rtl8xxxu_priv *priv,
				     int result[][8], int t)
{
	struct device *dev = &priv->udev->dev;
	u32 i, val32;
	int path_a_ok, path_b_ok;
	int retry = 2;
	const u32 adda_regs[RTL8XXXU_ADDA_REGS] = {
		REG_FPGA0_XCD_SWITCH_CTRL, REG_BLUETOOTH,
		REG_RX_WAIT_CCA, REG_TX_CCK_RFON,
		REG_TX_CCK_BBON, REG_TX_OFDM_RFON,
		REG_TX_OFDM_BBON, REG_TX_TO_RX,
		REG_TX_TO_TX, REG_RX_CCK,
		REG_RX_OFDM, REG_RX_WAIT_RIFS,
		REG_RX_TO_RX, REG_STANDBY,
		REG_SLEEP, REG_PMPD_ANAEN
	};
	const u32 iqk_mac_regs[RTL8XXXU_MAC_REGS] = {
		REG_TXPAUSE, REG_BEACON_CTRL,
		REG_BEACON_CTRL_1, REG_GPIO_MUXCFG
	};
	const u32 iqk_bb_regs[RTL8XXXU_BB_REGS] = {
		REG_OFDM0_TRX_PATH_ENABLE, REG_OFDM0_TR_MUX_PAR,
		REG_FPGA0_XCD_RF_SW_CTRL, REG_CONFIG_ANT_A, REG_CONFIG_ANT_B,
		REG_FPGA0_XAB_RF_SW_CTRL, REG_FPGA0_XA_RF_INT_OE,
		REG_FPGA0_XB_RF_INT_OE, REG_FPGA0_RF_MODE
	};

	/*
	 * Note: IQ calibration must be performed after loading
	 *       PHY_REG.txt , and radio_a, radio_b.txt
	 */

	if (t == 0) {
		/* Save ADDA parameters, turn Path A ADDA on */
		rtl8xxxu_save_regs(priv, adda_regs, priv->adda_backup,
				   RTL8XXXU_ADDA_REGS);
		rtl8xxxu_save_mac_regs(priv, iqk_mac_regs, priv->mac_backup);
		rtl8xxxu_save_regs(priv, iqk_bb_regs,
				   priv->bb_backup, RTL8XXXU_BB_REGS);
	}

	rtl8xxxu_path_adda_on(priv, adda_regs, true);

	if (t == 0) {
		val32 = rtl8xxxu_read32(priv, REG_FPGA0_XA_HSSI_PARM1);
		if (val32 & FPGA0_HSSI_PARM1_PI)
			priv->pi_enabled = 1;
	}

	if (!priv->pi_enabled) {
		/* Switch BB to PI mode to do IQ Calibration. */
		rtl8xxxu_write32(priv, REG_FPGA0_XA_HSSI_PARM1, 0x01000100);
		rtl8xxxu_write32(priv, REG_FPGA0_XB_HSSI_PARM1, 0x01000100);
	}

	val32 = rtl8xxxu_read32(priv, REG_FPGA0_RF_MODE);
	val32 &= ~FPGA_RF_MODE_CCK;
	rtl8xxxu_write32(priv, REG_FPGA0_RF_MODE, val32);

	rtl8xxxu_write32(priv, REG_OFDM0_TRX_PATH_ENABLE, 0x03a05600);
	rtl8xxxu_write32(priv, REG_OFDM0_TR_MUX_PAR, 0x000800e4);
	rtl8xxxu_write32(priv, REG_FPGA0_XCD_RF_SW_CTRL, 0x22204000);

	if (!priv->no_pape) {
		val32 = rtl8xxxu_read32(priv, REG_FPGA0_XAB_RF_SW_CTRL);
		val32 |= (FPGA0_RF_PAPE |
			  (FPGA0_RF_PAPE << FPGA0_RF_BD_CTRL_SHIFT));
		rtl8xxxu_write32(priv, REG_FPGA0_XAB_RF_SW_CTRL, val32);
	}

	val32 = rtl8xxxu_read32(priv, REG_FPGA0_XA_RF_INT_OE);
	val32 &= ~BIT(10);
	rtl8xxxu_write32(priv, REG_FPGA0_XA_RF_INT_OE, val32);
	val32 = rtl8xxxu_read32(priv, REG_FPGA0_XB_RF_INT_OE);
	val32 &= ~BIT(10);
	rtl8xxxu_write32(priv, REG_FPGA0_XB_RF_INT_OE, val32);

	if (priv->tx_paths > 1) {
		rtl8xxxu_write32(priv, REG_FPGA0_XA_LSSI_PARM, 0x00010000);
		rtl8xxxu_write32(priv, REG_FPGA0_XB_LSSI_PARM, 0x00010000);
	}

	/* MAC settings */
	rtl8xxxu_mac_calibration(priv, iqk_mac_regs, priv->mac_backup);

	/* Page B init */
	rtl8xxxu_write32(priv, REG_CONFIG_ANT_A, 0x00080000);

	if (priv->tx_paths > 1)
		rtl8xxxu_write32(priv, REG_CONFIG_ANT_B, 0x00080000);

	/* IQ calibration setting */
	rtl8xxxu_write32(priv, REG_FPGA0_IQK, 0x80800000);
	rtl8xxxu_write32(priv, REG_TX_IQK, 0x01007c00);
	rtl8xxxu_write32(priv, REG_RX_IQK, 0x01004800);

	for (i = 0; i < retry; i++) {
		path_a_ok = rtl8xxxu_iqk_path_a(priv);
		if (path_a_ok == 0x03) {
			val32 = rtl8xxxu_read32(priv,
						REG_TX_POWER_BEFORE_IQK_A);
			result[t][0] = (val32 >> 16) & 0x3ff;
			val32 = rtl8xxxu_read32(priv,
						REG_TX_POWER_AFTER_IQK_A);
			result[t][1] = (val32 >> 16) & 0x3ff;
			val32 = rtl8xxxu_read32(priv,
						REG_RX_POWER_BEFORE_IQK_A_2);
			result[t][2] = (val32 >> 16) & 0x3ff;
			val32 = rtl8xxxu_read32(priv,
						REG_RX_POWER_AFTER_IQK_A_2);
			result[t][3] = (val32 >> 16) & 0x3ff;
			break;
		} else if (i == (retry - 1) && path_a_ok == 0x01) {
			/* TX IQK OK */
			dev_dbg(dev, "%s: Path A IQK Only Tx Success!!\n",
				__func__);

			val32 = rtl8xxxu_read32(priv,
						REG_TX_POWER_BEFORE_IQK_A);
			result[t][0] = (val32 >> 16) & 0x3ff;
			val32 = rtl8xxxu_read32(priv,
						REG_TX_POWER_AFTER_IQK_A);
			result[t][1] = (val32 >> 16) & 0x3ff;
		}
	}

	if (!path_a_ok)
		dev_dbg(dev, "%s: Path A IQK failed!\n", __func__);

	if (priv->tx_paths > 1) {
		/*
		 * Path A into standby
		 */
		rtl8xxxu_write32(priv, REG_FPGA0_IQK, 0x0);
		rtl8xxxu_write32(priv, REG_FPGA0_XA_LSSI_PARM, 0x00010000);
		rtl8xxxu_write32(priv, REG_FPGA0_IQK, 0x80800000);

		/* Turn Path B ADDA on */
		rtl8xxxu_path_adda_on(priv, adda_regs, false);

		for (i = 0; i < retry; i++) {
			path_b_ok = rtl8xxxu_iqk_path_b(priv);
			if (path_b_ok == 0x03) {
				val32 = rtl8xxxu_read32(priv, REG_TX_POWER_BEFORE_IQK_B);
				result[t][4] = (val32 >> 16) & 0x3ff;
				val32 = rtl8xxxu_read32(priv, REG_TX_POWER_AFTER_IQK_B);
				result[t][5] = (val32 >> 16) & 0x3ff;
				val32 = rtl8xxxu_read32(priv, REG_RX_POWER_BEFORE_IQK_B_2);
				result[t][6] = (val32 >> 16) & 0x3ff;
				val32 = rtl8xxxu_read32(priv, REG_RX_POWER_AFTER_IQK_B_2);
				result[t][7] = (val32 >> 16) & 0x3ff;
				break;
			} else if (i == (retry - 1) && path_b_ok == 0x01) {
				/* TX IQK OK */
				val32 = rtl8xxxu_read32(priv, REG_TX_POWER_BEFORE_IQK_B);
				result[t][4] = (val32 >> 16) & 0x3ff;
				val32 = rtl8xxxu_read32(priv, REG_TX_POWER_AFTER_IQK_B);
				result[t][5] = (val32 >> 16) & 0x3ff;
			}
		}

		if (!path_b_ok)
			dev_dbg(dev, "%s: Path B IQK failed!\n", __func__);
	}

	/* Back to BB mode, load original value */
	rtl8xxxu_write32(priv, REG_FPGA0_IQK, 0);

	if (t) {
		if (!priv->pi_enabled) {
			/*
			 * Switch back BB to SI mode after finishing
			 * IQ Calibration
			 */
			val32 = 0x01000000;
			rtl8xxxu_write32(priv, REG_FPGA0_XA_HSSI_PARM1, val32);
			rtl8xxxu_write32(priv, REG_FPGA0_XB_HSSI_PARM1, val32);
		}

		/* Reload ADDA power saving parameters */
		rtl8xxxu_restore_regs(priv, adda_regs, priv->adda_backup,
				      RTL8XXXU_ADDA_REGS);

		/* Reload MAC parameters */
		rtl8xxxu_restore_mac_regs(priv, iqk_mac_regs, priv->mac_backup);

		/* Reload BB parameters */
		rtl8xxxu_restore_regs(priv, iqk_bb_regs,
				      priv->bb_backup, RTL8XXXU_BB_REGS);

		/* Restore RX initial gain */
		rtl8xxxu_write32(priv, REG_FPGA0_XA_LSSI_PARM, 0x00032ed3);

		if (priv->tx_paths > 1) {
			rtl8xxxu_write32(priv, REG_FPGA0_XB_LSSI_PARM,
					 0x00032ed3);
		}

		/* Load 0xe30 IQC default value */
		rtl8xxxu_write32(priv, REG_TX_IQK_TONE_A, 0x01008c00);
		rtl8xxxu_write32(priv, REG_RX_IQK_TONE_A, 0x01008c00);
	}
}