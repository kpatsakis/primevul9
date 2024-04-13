static int rtl8xxxu_probe(struct usb_interface *interface,
			  const struct usb_device_id *id)
{
	struct rtl8xxxu_priv *priv;
	struct ieee80211_hw *hw;
	struct usb_device *udev;
	struct ieee80211_supported_band *sband;
	int ret;
	int untested = 1;

	udev = usb_get_dev(interface_to_usbdev(interface));

	switch (id->idVendor) {
	case USB_VENDOR_ID_REALTEK:
		switch(id->idProduct) {
		case 0x1724:
		case 0x8176:
		case 0x8178:
		case 0x817f:
		case 0x818b:
			untested = 0;
			break;
		}
		break;
	case 0x7392:
		if (id->idProduct == 0x7811)
			untested = 0;
		break;
	case 0x050d:
		if (id->idProduct == 0x1004)
			untested = 0;
		break;
	case 0x20f4:
		if (id->idProduct == 0x648b)
			untested = 0;
		break;
	case 0x2001:
		if (id->idProduct == 0x3308)
			untested = 0;
		break;
	case 0x2357:
		if (id->idProduct == 0x0109)
			untested = 0;
		break;
	default:
		break;
	}

	if (untested) {
		rtl8xxxu_debug |= RTL8XXXU_DEBUG_EFUSE;
		dev_info(&udev->dev,
			 "This Realtek USB WiFi dongle (0x%04x:0x%04x) is untested!\n",
			 id->idVendor, id->idProduct);
		dev_info(&udev->dev,
			 "Please report results to Jes.Sorensen@gmail.com\n");
	}

	hw = ieee80211_alloc_hw(sizeof(struct rtl8xxxu_priv), &rtl8xxxu_ops);
	if (!hw) {
		ret = -ENOMEM;
		priv = NULL;
		goto exit;
	}

	priv = hw->priv;
	priv->hw = hw;
	priv->udev = udev;
	priv->fops = (struct rtl8xxxu_fileops *)id->driver_info;
	mutex_init(&priv->usb_buf_mutex);
	mutex_init(&priv->h2c_mutex);
	INIT_LIST_HEAD(&priv->tx_urb_free_list);
	spin_lock_init(&priv->tx_urb_lock);
	INIT_LIST_HEAD(&priv->rx_urb_pending_list);
	spin_lock_init(&priv->rx_urb_lock);
	INIT_WORK(&priv->rx_urb_wq, rtl8xxxu_rx_urb_work);

	usb_set_intfdata(interface, hw);

	ret = rtl8xxxu_parse_usb(priv, interface);
	if (ret)
		goto exit;

	ret = rtl8xxxu_identify_chip(priv);
	if (ret) {
		dev_err(&udev->dev, "Fatal - failed to identify chip\n");
		goto exit;
	}

	ret = rtl8xxxu_read_efuse(priv);
	if (ret) {
		dev_err(&udev->dev, "Fatal - failed to read EFuse\n");
		goto exit;
	}

	ret = priv->fops->parse_efuse(priv);
	if (ret) {
		dev_err(&udev->dev, "Fatal - failed to parse EFuse\n");
		goto exit;
	}

	rtl8xxxu_print_chipinfo(priv);

	ret = priv->fops->load_firmware(priv);
	if (ret) {
		dev_err(&udev->dev, "Fatal - failed to load firmware\n");
		goto exit;
	}

	ret = rtl8xxxu_init_device(hw);
	if (ret)
		goto exit;

	hw->wiphy->max_scan_ssids = 1;
	hw->wiphy->max_scan_ie_len = IEEE80211_MAX_DATA_LEN;
	hw->wiphy->interface_modes = BIT(NL80211_IFTYPE_STATION);
	hw->queues = 4;

	sband = &rtl8xxxu_supported_band;
	sband->ht_cap.ht_supported = true;
	sband->ht_cap.ampdu_factor = IEEE80211_HT_MAX_AMPDU_64K;
	sband->ht_cap.ampdu_density = IEEE80211_HT_MPDU_DENSITY_16;
	sband->ht_cap.cap = IEEE80211_HT_CAP_SGI_20 | IEEE80211_HT_CAP_SGI_40;
	memset(&sband->ht_cap.mcs, 0, sizeof(sband->ht_cap.mcs));
	sband->ht_cap.mcs.rx_mask[0] = 0xff;
	sband->ht_cap.mcs.rx_mask[4] = 0x01;
	if (priv->rf_paths > 1) {
		sband->ht_cap.mcs.rx_mask[1] = 0xff;
		sband->ht_cap.cap |= IEEE80211_HT_CAP_SGI_40;
	}
	sband->ht_cap.mcs.tx_params = IEEE80211_HT_MCS_TX_DEFINED;
	/*
	 * Some APs will negotiate HT20_40 in a noisy environment leading
	 * to miserable performance. Rather than defaulting to this, only
	 * enable it if explicitly requested at module load time.
	 */
	if (rtl8xxxu_ht40_2g) {
		dev_info(&udev->dev, "Enabling HT_20_40 on the 2.4GHz band\n");
		sband->ht_cap.cap |= IEEE80211_HT_CAP_SUP_WIDTH_20_40;
	}
	hw->wiphy->bands[NL80211_BAND_2GHZ] = sband;

	hw->wiphy->rts_threshold = 2347;

	SET_IEEE80211_DEV(priv->hw, &interface->dev);
	SET_IEEE80211_PERM_ADDR(hw, priv->mac_addr);

	hw->extra_tx_headroom = priv->fops->tx_desc_size;
	ieee80211_hw_set(hw, SIGNAL_DBM);
	/*
	 * The firmware handles rate control
	 */
	ieee80211_hw_set(hw, HAS_RATE_CONTROL);
	ieee80211_hw_set(hw, AMPDU_AGGREGATION);

	wiphy_ext_feature_set(hw->wiphy, NL80211_EXT_FEATURE_CQM_RSSI_LIST);

	ret = ieee80211_register_hw(priv->hw);
	if (ret) {
		dev_err(&udev->dev, "%s: Failed to register: %i\n",
			__func__, ret);
		goto exit;
	}

	return 0;

exit:
	usb_set_intfdata(interface, NULL);

	if (priv) {
		kfree(priv->fw_data);
		mutex_destroy(&priv->usb_buf_mutex);
		mutex_destroy(&priv->h2c_mutex);
	}
	usb_put_dev(udev);

	ieee80211_free_hw(hw);

	return ret;
}