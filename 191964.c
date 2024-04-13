static int rtl8xxxu_init_device(struct ieee80211_hw *hw)
{
	struct rtl8xxxu_priv *priv = hw->priv;
	struct device *dev = &priv->udev->dev;
	struct rtl8xxxu_fileops *fops = priv->fops;
	bool macpower;
	int ret;
	u8 val8;
	u16 val16;
	u32 val32;

	/* Check if MAC is already powered on */
	val8 = rtl8xxxu_read8(priv, REG_CR);
	val16 = rtl8xxxu_read16(priv, REG_SYS_CLKR);

	/*
	 * Fix 92DU-VC S3 hang with the reason is that secondary mac is not
	 * initialized. First MAC returns 0xea, second MAC returns 0x00
	 */
	if (val8 == 0xea || !(val16 & SYS_CLK_MAC_CLK_ENABLE))
		macpower = false;
	else
		macpower = true;

	ret = fops->power_on(priv);
	if (ret < 0) {
		dev_warn(dev, "%s: Failed power on\n", __func__);
		goto exit;
	}

	if (!macpower)
		rtl8xxxu_init_queue_reserved_page(priv);

	ret = rtl8xxxu_init_queue_priority(priv);
	dev_dbg(dev, "%s: init_queue_priority %i\n", __func__, ret);
	if (ret)
		goto exit;

	/*
	 * Set RX page boundary
	 */
	rtl8xxxu_write16(priv, REG_TRXFF_BNDY + 2, fops->trxff_boundary);

	ret = rtl8xxxu_download_firmware(priv);
	dev_dbg(dev, "%s: download_firmware %i\n", __func__, ret);
	if (ret)
		goto exit;
	ret = rtl8xxxu_start_firmware(priv);
	dev_dbg(dev, "%s: start_firmware %i\n", __func__, ret);
	if (ret)
		goto exit;

	if (fops->phy_init_antenna_selection)
		fops->phy_init_antenna_selection(priv);

	ret = rtl8xxxu_init_mac(priv);

	dev_dbg(dev, "%s: init_mac %i\n", __func__, ret);
	if (ret)
		goto exit;

	ret = rtl8xxxu_init_phy_bb(priv);
	dev_dbg(dev, "%s: init_phy_bb %i\n", __func__, ret);
	if (ret)
		goto exit;

	ret = fops->init_phy_rf(priv);
	if (ret)
		goto exit;

	/* RFSW Control - clear bit 14 ?? */
	if (priv->rtl_chip != RTL8723B && priv->rtl_chip != RTL8192E)
		rtl8xxxu_write32(priv, REG_FPGA0_TX_INFO, 0x00000003);

	val32 = FPGA0_RF_TRSW | FPGA0_RF_TRSWB | FPGA0_RF_ANTSW |
		FPGA0_RF_ANTSWB |
		((FPGA0_RF_ANTSW | FPGA0_RF_ANTSWB) << FPGA0_RF_BD_CTRL_SHIFT);
	if (!priv->no_pape) {
		val32 |= (FPGA0_RF_PAPE |
			  (FPGA0_RF_PAPE << FPGA0_RF_BD_CTRL_SHIFT));
	}
	rtl8xxxu_write32(priv, REG_FPGA0_XAB_RF_SW_CTRL, val32);

	/* 0x860[6:5]= 00 - why? - this sets antenna B */
	if (priv->rtl_chip != RTL8192E)
		rtl8xxxu_write32(priv, REG_FPGA0_XA_RF_INT_OE, 0x66f60210);

	if (!macpower) {
		/*
		 * Set TX buffer boundary
		 */
		val8 = fops->total_page_num + 1;

		rtl8xxxu_write8(priv, REG_TXPKTBUF_BCNQ_BDNY, val8);
		rtl8xxxu_write8(priv, REG_TXPKTBUF_MGQ_BDNY, val8);
		rtl8xxxu_write8(priv, REG_TXPKTBUF_WMAC_LBK_BF_HD, val8);
		rtl8xxxu_write8(priv, REG_TRXFF_BNDY, val8);
		rtl8xxxu_write8(priv, REG_TDECTRL + 1, val8);
	}

	/*
	 * The vendor drivers set PBP for all devices, except 8192e.
	 * There is no explanation for this in any of the sources.
	 */
	val8 = (fops->pbp_rx << PBP_PAGE_SIZE_RX_SHIFT) |
		(fops->pbp_tx << PBP_PAGE_SIZE_TX_SHIFT);
	if (priv->rtl_chip != RTL8192E)
		rtl8xxxu_write8(priv, REG_PBP, val8);

	dev_dbg(dev, "%s: macpower %i\n", __func__, macpower);
	if (!macpower) {
		ret = fops->llt_init(priv);
		if (ret) {
			dev_warn(dev, "%s: LLT table init failed\n", __func__);
			goto exit;
		}

		/*
		 * Chip specific quirks
		 */
		fops->usb_quirks(priv);

		/*
		 * Enable TX report and TX report timer for 8723bu/8188eu/...
		 */
		if (fops->has_tx_report) {
			val8 = rtl8xxxu_read8(priv, REG_TX_REPORT_CTRL);
			val8 |= TX_REPORT_CTRL_TIMER_ENABLE;
			rtl8xxxu_write8(priv, REG_TX_REPORT_CTRL, val8);
			/* Set MAX RPT MACID */
			rtl8xxxu_write8(priv, REG_TX_REPORT_CTRL + 1, 0x02);
			/* TX report Timer. Unit: 32us */
			rtl8xxxu_write16(priv, REG_TX_REPORT_TIME, 0xcdf0);

			/* tmp ps ? */
			val8 = rtl8xxxu_read8(priv, 0xa3);
			val8 &= 0xf8;
			rtl8xxxu_write8(priv, 0xa3, val8);
		}
	}

	/*
	 * Unit in 8 bytes, not obvious what it is used for
	 */
	rtl8xxxu_write8(priv, REG_RX_DRVINFO_SZ, 4);

	if (priv->rtl_chip == RTL8192E) {
		rtl8xxxu_write32(priv, REG_HIMR0, 0x00);
		rtl8xxxu_write32(priv, REG_HIMR1, 0x00);
	} else {
		/*
		 * Enable all interrupts - not obvious USB needs to do this
		 */
		rtl8xxxu_write32(priv, REG_HISR, 0xffffffff);
		rtl8xxxu_write32(priv, REG_HIMR, 0xffffffff);
	}

	rtl8xxxu_set_mac(priv);
	rtl8xxxu_set_linktype(priv, NL80211_IFTYPE_STATION);

	/*
	 * Configure initial WMAC settings
	 */
	val32 = RCR_ACCEPT_PHYS_MATCH | RCR_ACCEPT_MCAST | RCR_ACCEPT_BCAST |
		RCR_ACCEPT_MGMT_FRAME | RCR_HTC_LOC_CTRL |
		RCR_APPEND_PHYSTAT | RCR_APPEND_ICV | RCR_APPEND_MIC;
	rtl8xxxu_write32(priv, REG_RCR, val32);

	/*
	 * Accept all multicast
	 */
	rtl8xxxu_write32(priv, REG_MAR, 0xffffffff);
	rtl8xxxu_write32(priv, REG_MAR + 4, 0xffffffff);

	/*
	 * Init adaptive controls
	 */
	val32 = rtl8xxxu_read32(priv, REG_RESPONSE_RATE_SET);
	val32 &= ~RESPONSE_RATE_BITMAP_ALL;
	val32 |= RESPONSE_RATE_RRSR_CCK_ONLY_1M;
	rtl8xxxu_write32(priv, REG_RESPONSE_RATE_SET, val32);

	/* CCK = 0x0a, OFDM = 0x10 */
	rtl8xxxu_set_spec_sifs(priv, 0x10, 0x10);
	rtl8xxxu_set_retry(priv, 0x30, 0x30);
	rtl8xxxu_set_spec_sifs(priv, 0x0a, 0x10);

	/*
	 * Init EDCA
	 */
	rtl8xxxu_write16(priv, REG_MAC_SPEC_SIFS, 0x100a);

	/* Set CCK SIFS */
	rtl8xxxu_write16(priv, REG_SIFS_CCK, 0x100a);

	/* Set OFDM SIFS */
	rtl8xxxu_write16(priv, REG_SIFS_OFDM, 0x100a);

	/* TXOP */
	rtl8xxxu_write32(priv, REG_EDCA_BE_PARAM, 0x005ea42b);
	rtl8xxxu_write32(priv, REG_EDCA_BK_PARAM, 0x0000a44f);
	rtl8xxxu_write32(priv, REG_EDCA_VI_PARAM, 0x005ea324);
	rtl8xxxu_write32(priv, REG_EDCA_VO_PARAM, 0x002fa226);

	/* Set data auto rate fallback retry count */
	rtl8xxxu_write32(priv, REG_DARFRC, 0x00000000);
	rtl8xxxu_write32(priv, REG_DARFRC + 4, 0x10080404);
	rtl8xxxu_write32(priv, REG_RARFRC, 0x04030201);
	rtl8xxxu_write32(priv, REG_RARFRC + 4, 0x08070605);

	val8 = rtl8xxxu_read8(priv, REG_FWHW_TXQ_CTRL);
	val8 |= FWHW_TXQ_CTRL_AMPDU_RETRY;
	rtl8xxxu_write8(priv, REG_FWHW_TXQ_CTRL, val8);

	/*  Set ACK timeout */
	rtl8xxxu_write8(priv, REG_ACKTO, 0x40);

	/*
	 * Initialize beacon parameters
	 */
	val16 = BEACON_DISABLE_TSF_UPDATE | (BEACON_DISABLE_TSF_UPDATE << 8);
	rtl8xxxu_write16(priv, REG_BEACON_CTRL, val16);
	rtl8xxxu_write16(priv, REG_TBTT_PROHIBIT, 0x6404);
	rtl8xxxu_write8(priv, REG_DRIVER_EARLY_INT, DRIVER_EARLY_INT_TIME);
	rtl8xxxu_write8(priv, REG_BEACON_DMA_TIME, BEACON_DMA_ATIME_INT_TIME);
	rtl8xxxu_write16(priv, REG_BEACON_TCFG, 0x660F);

	/*
	 * Initialize burst parameters
	 */
	if (priv->rtl_chip == RTL8723B) {
		/*
		 * For USB high speed set 512B packets
		 */
		val8 = rtl8xxxu_read8(priv, REG_RXDMA_PRO_8723B);
		val8 &= ~(BIT(4) | BIT(5));
		val8 |= BIT(4);
		val8 |= BIT(1) | BIT(2) | BIT(3);
		rtl8xxxu_write8(priv, REG_RXDMA_PRO_8723B, val8);

		/*
		 * For USB high speed set 512B packets
		 */
		val8 = rtl8xxxu_read8(priv, REG_HT_SINGLE_AMPDU_8723B);
		val8 |= BIT(7);
		rtl8xxxu_write8(priv, REG_HT_SINGLE_AMPDU_8723B, val8);

		rtl8xxxu_write16(priv, REG_MAX_AGGR_NUM, 0x0c14);
		rtl8xxxu_write8(priv, REG_AMPDU_MAX_TIME_8723B, 0x5e);
		rtl8xxxu_write32(priv, REG_AGGLEN_LMT, 0xffffffff);
		rtl8xxxu_write8(priv, REG_RX_PKT_LIMIT, 0x18);
		rtl8xxxu_write8(priv, REG_PIFS, 0x00);
		rtl8xxxu_write8(priv, REG_USTIME_TSF_8723B, 0x50);
		rtl8xxxu_write8(priv, REG_USTIME_EDCA, 0x50);

		val8 = rtl8xxxu_read8(priv, REG_RSV_CTRL);
		val8 |= BIT(5) | BIT(6);
		rtl8xxxu_write8(priv, REG_RSV_CTRL, val8);
	}

	if (fops->init_aggregation)
		fops->init_aggregation(priv);

	/*
	 * Enable CCK and OFDM block
	 */
	val32 = rtl8xxxu_read32(priv, REG_FPGA0_RF_MODE);
	val32 |= (FPGA_RF_MODE_CCK | FPGA_RF_MODE_OFDM);
	rtl8xxxu_write32(priv, REG_FPGA0_RF_MODE, val32);

	/*
	 * Invalidate all CAM entries - bit 30 is undocumented
	 */
	rtl8xxxu_write32(priv, REG_CAM_CMD, CAM_CMD_POLLING | BIT(30));

	/*
	 * Start out with default power levels for channel 6, 20MHz
	 */
	fops->set_tx_power(priv, 1, false);

	/* Let the 8051 take control of antenna setting */
	if (priv->rtl_chip != RTL8192E) {
		val8 = rtl8xxxu_read8(priv, REG_LEDCFG2);
		val8 |= LEDCFG2_DPDT_SELECT;
		rtl8xxxu_write8(priv, REG_LEDCFG2, val8);
	}

	rtl8xxxu_write8(priv, REG_HWSEQ_CTRL, 0xff);

	/* Disable BAR - not sure if this has any effect on USB */
	rtl8xxxu_write32(priv, REG_BAR_MODE_CTRL, 0x0201ffff);

	rtl8xxxu_write16(priv, REG_FAST_EDCA_CTRL, 0);

	if (fops->init_statistics)
		fops->init_statistics(priv);

	if (priv->rtl_chip == RTL8192E) {
		/*
		 * 0x4c6[3] 1: RTS BW = Data BW
		 * 0: RTS BW depends on CCA / secondary CCA result.
		 */
		val8 = rtl8xxxu_read8(priv, REG_QUEUE_CTRL);
		val8 &= ~BIT(3);
		rtl8xxxu_write8(priv, REG_QUEUE_CTRL, val8);
		/*
		 * Reset USB mode switch setting
		 */
		rtl8xxxu_write8(priv, REG_ACLK_MON, 0x00);
	}

	rtl8723a_phy_lc_calibrate(priv);

	fops->phy_iq_calibrate(priv);

	/*
	 * This should enable thermal meter
	 */
	if (fops->gen2_thermal_meter)
		rtl8xxxu_write_rfreg(priv,
				     RF_A, RF6052_REG_T_METER_8723B, 0x37cf8);
	else
		rtl8xxxu_write_rfreg(priv, RF_A, RF6052_REG_T_METER, 0x60);

	/* Set NAV_UPPER to 30000us */
	val8 = ((30000 + NAV_UPPER_UNIT - 1) / NAV_UPPER_UNIT);
	rtl8xxxu_write8(priv, REG_NAV_UPPER, val8);

	if (priv->rtl_chip == RTL8723A) {
		/*
		 * 2011/03/09 MH debug only, UMC-B cut pass 2500 S5 test,
		 * but we need to find root cause.
		 * This is 8723au only.
		 */
		val32 = rtl8xxxu_read32(priv, REG_FPGA0_RF_MODE);
		if ((val32 & 0xff000000) != 0x83000000) {
			val32 |= FPGA_RF_MODE_CCK;
			rtl8xxxu_write32(priv, REG_FPGA0_RF_MODE, val32);
		}
	} else if (priv->rtl_chip == RTL8192E) {
		rtl8xxxu_write8(priv, REG_USB_HRPWM, 0x00);
	}

	val32 = rtl8xxxu_read32(priv, REG_FWHW_TXQ_CTRL);
	val32 |= FWHW_TXQ_CTRL_XMIT_MGMT_ACK;
	/* ack for xmit mgmt frames. */
	rtl8xxxu_write32(priv, REG_FWHW_TXQ_CTRL, val32);

	if (priv->rtl_chip == RTL8192E) {
		/*
		 * Fix LDPC rx hang issue.
		 */
		val32 = rtl8xxxu_read32(priv, REG_AFE_MISC);
		rtl8xxxu_write8(priv, REG_8192E_LDOV12_CTRL, 0x75);
		val32 &= 0xfff00fff;
		val32 |= 0x0007e000;
		rtl8xxxu_write32(priv, REG_AFE_MISC, val32);
	}
exit:
	return ret;
}