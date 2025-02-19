mwifiex_is_network_compatible(struct mwifiex_private *priv,
			      struct mwifiex_bssdescriptor *bss_desc, u32 mode)
{
	struct mwifiex_adapter *adapter = priv->adapter;

	bss_desc->disable_11n = false;

	/* Don't check for compatibility if roaming */
	if (priv->media_connected &&
	    (priv->bss_mode == NL80211_IFTYPE_STATION) &&
	    (bss_desc->bss_mode == NL80211_IFTYPE_STATION))
		return 0;

	if (priv->wps.session_enable) {
		mwifiex_dbg(adapter, IOCTL,
			    "info: return success directly in WPS period\n");
		return 0;
	}

	if (bss_desc->chan_sw_ie_present) {
		mwifiex_dbg(adapter, INFO,
			    "Don't connect to AP with WLAN_EID_CHANNEL_SWITCH\n");
		return -1;
	}

	if (mwifiex_is_bss_wapi(priv, bss_desc)) {
		mwifiex_dbg(adapter, INFO,
			    "info: return success for WAPI AP\n");
		return 0;
	}

	if (bss_desc->bss_mode == mode) {
		if (mwifiex_is_bss_no_sec(priv, bss_desc)) {
			/* No security */
			return 0;
		} else if (mwifiex_is_bss_static_wep(priv, bss_desc)) {
			/* Static WEP enabled */
			mwifiex_dbg(adapter, INFO,
				    "info: Disable 11n in WEP mode.\n");
			bss_desc->disable_11n = true;
			return 0;
		} else if (mwifiex_is_bss_wpa(priv, bss_desc)) {
			/* WPA enabled */
			if (((priv->adapter->config_bands & BAND_GN ||
			      priv->adapter->config_bands & BAND_AN) &&
			     bss_desc->bcn_ht_cap) &&
			    !mwifiex_is_wpa_oui_present(bss_desc,
							 CIPHER_SUITE_CCMP)) {

				if (mwifiex_is_wpa_oui_present
						(bss_desc, CIPHER_SUITE_TKIP)) {
					mwifiex_dbg(adapter, INFO,
						    "info: Disable 11n if AES\t"
						    "is not supported by AP\n");
					bss_desc->disable_11n = true;
				} else {
					return -1;
				}
			}
			return 0;
		} else if (mwifiex_is_bss_wpa2(priv, bss_desc)) {
			/* WPA2 enabled */
			if (((priv->adapter->config_bands & BAND_GN ||
			      priv->adapter->config_bands & BAND_AN) &&
			     bss_desc->bcn_ht_cap) &&
			    !mwifiex_is_rsn_oui_present(bss_desc,
							CIPHER_SUITE_CCMP)) {

				if (mwifiex_is_rsn_oui_present
						(bss_desc, CIPHER_SUITE_TKIP)) {
					mwifiex_dbg(adapter, INFO,
						    "info: Disable 11n if AES\t"
						    "is not supported by AP\n");
					bss_desc->disable_11n = true;
				} else {
					return -1;
				}
			}
			return 0;
		} else if (mwifiex_is_bss_adhoc_aes(priv, bss_desc)) {
			/* Ad-hoc AES enabled */
			return 0;
		} else if (mwifiex_is_bss_dynamic_wep(priv, bss_desc)) {
			/* Dynamic WEP enabled */
			return 0;
		}

		/* Security doesn't match */
		dbg_security_flags(ERROR, "failed", priv, bss_desc);
		return -1;
	}

	/* Mode doesn't match */
	return -1;
}