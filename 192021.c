static int rtl8xxxu_identify_chip(struct rtl8xxxu_priv *priv)
{
	struct device *dev = &priv->udev->dev;
	u32 val32, bonding;
	u16 val16;

	val32 = rtl8xxxu_read32(priv, REG_SYS_CFG);
	priv->chip_cut = (val32 & SYS_CFG_CHIP_VERSION_MASK) >>
		SYS_CFG_CHIP_VERSION_SHIFT;
	if (val32 & SYS_CFG_TRP_VAUX_EN) {
		dev_info(dev, "Unsupported test chip\n");
		return -ENOTSUPP;
	}

	if (val32 & SYS_CFG_BT_FUNC) {
		if (priv->chip_cut >= 3) {
			sprintf(priv->chip_name, "8723BU");
			priv->rtl_chip = RTL8723B;
		} else {
			sprintf(priv->chip_name, "8723AU");
			priv->usb_interrupts = 1;
			priv->rtl_chip = RTL8723A;
		}

		priv->rf_paths = 1;
		priv->rx_paths = 1;
		priv->tx_paths = 1;

		val32 = rtl8xxxu_read32(priv, REG_MULTI_FUNC_CTRL);
		if (val32 & MULTI_WIFI_FUNC_EN)
			priv->has_wifi = 1;
		if (val32 & MULTI_BT_FUNC_EN)
			priv->has_bluetooth = 1;
		if (val32 & MULTI_GPS_FUNC_EN)
			priv->has_gps = 1;
		priv->is_multi_func = 1;
	} else if (val32 & SYS_CFG_TYPE_ID) {
		bonding = rtl8xxxu_read32(priv, REG_HPON_FSM);
		bonding &= HPON_FSM_BONDING_MASK;
		if (priv->fops->tx_desc_size ==
		    sizeof(struct rtl8xxxu_txdesc40)) {
			if (bonding == HPON_FSM_BONDING_1T2R) {
				sprintf(priv->chip_name, "8191EU");
				priv->rf_paths = 2;
				priv->rx_paths = 2;
				priv->tx_paths = 1;
				priv->rtl_chip = RTL8191E;
			} else {
				sprintf(priv->chip_name, "8192EU");
				priv->rf_paths = 2;
				priv->rx_paths = 2;
				priv->tx_paths = 2;
				priv->rtl_chip = RTL8192E;
			}
		} else if (bonding == HPON_FSM_BONDING_1T2R) {
			sprintf(priv->chip_name, "8191CU");
			priv->rf_paths = 2;
			priv->rx_paths = 2;
			priv->tx_paths = 1;
			priv->usb_interrupts = 1;
			priv->rtl_chip = RTL8191C;
		} else {
			sprintf(priv->chip_name, "8192CU");
			priv->rf_paths = 2;
			priv->rx_paths = 2;
			priv->tx_paths = 2;
			priv->usb_interrupts = 1;
			priv->rtl_chip = RTL8192C;
		}
		priv->has_wifi = 1;
	} else {
		sprintf(priv->chip_name, "8188CU");
		priv->rf_paths = 1;
		priv->rx_paths = 1;
		priv->tx_paths = 1;
		priv->rtl_chip = RTL8188C;
		priv->usb_interrupts = 1;
		priv->has_wifi = 1;
	}

	switch (priv->rtl_chip) {
	case RTL8188E:
	case RTL8192E:
	case RTL8723B:
		switch (val32 & SYS_CFG_VENDOR_EXT_MASK) {
		case SYS_CFG_VENDOR_ID_TSMC:
			sprintf(priv->chip_vendor, "TSMC");
			break;
		case SYS_CFG_VENDOR_ID_SMIC:
			sprintf(priv->chip_vendor, "SMIC");
			priv->vendor_smic = 1;
			break;
		case SYS_CFG_VENDOR_ID_UMC:
			sprintf(priv->chip_vendor, "UMC");
			priv->vendor_umc = 1;
			break;
		default:
			sprintf(priv->chip_vendor, "unknown");
		}
		break;
	default:
		if (val32 & SYS_CFG_VENDOR_ID) {
			sprintf(priv->chip_vendor, "UMC");
			priv->vendor_umc = 1;
		} else {
			sprintf(priv->chip_vendor, "TSMC");
		}
	}

	val32 = rtl8xxxu_read32(priv, REG_GPIO_OUTSTS);
	priv->rom_rev = (val32 & GPIO_RF_RL_ID) >> 28;

	val16 = rtl8xxxu_read16(priv, REG_NORMAL_SIE_EP_TX);
	if (val16 & NORMAL_SIE_EP_TX_HIGH_MASK) {
		priv->ep_tx_high_queue = 1;
		priv->ep_tx_count++;
	}

	if (val16 & NORMAL_SIE_EP_TX_NORMAL_MASK) {
		priv->ep_tx_normal_queue = 1;
		priv->ep_tx_count++;
	}

	if (val16 & NORMAL_SIE_EP_TX_LOW_MASK) {
		priv->ep_tx_low_queue = 1;
		priv->ep_tx_count++;
	}

	/*
	 * Fallback for devices that do not provide REG_NORMAL_SIE_EP_TX
	 */
	if (!priv->ep_tx_count) {
		switch (priv->nr_out_eps) {
		case 4:
		case 3:
			priv->ep_tx_low_queue = 1;
			priv->ep_tx_count++;
			/* fall through */
		case 2:
			priv->ep_tx_normal_queue = 1;
			priv->ep_tx_count++;
			/* fall through */
		case 1:
			priv->ep_tx_high_queue = 1;
			priv->ep_tx_count++;
			break;
		default:
			dev_info(dev, "Unsupported USB TX end-points\n");
			return -ENOTSUPP;
		}
	}

	return 0;
}