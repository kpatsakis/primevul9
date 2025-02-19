int mwifiex_update_bss_desc_with_ie(struct mwifiex_adapter *adapter,
				    struct mwifiex_bssdescriptor *bss_entry)
{
	int ret = 0;
	u8 element_id;
	struct ieee_types_fh_param_set *fh_param_set;
	struct ieee_types_ds_param_set *ds_param_set;
	struct ieee_types_cf_param_set *cf_param_set;
	struct ieee_types_ibss_param_set *ibss_param_set;
	u8 *current_ptr;
	u8 *rate;
	u8 element_len;
	u16 total_ie_len;
	u8 bytes_to_copy;
	u8 rate_size;
	u8 found_data_rate_ie;
	u32 bytes_left;
	struct ieee_types_vendor_specific *vendor_ie;
	const u8 wpa_oui[4] = { 0x00, 0x50, 0xf2, 0x01 };
	const u8 wmm_oui[4] = { 0x00, 0x50, 0xf2, 0x02 };

	found_data_rate_ie = false;
	rate_size = 0;
	current_ptr = bss_entry->beacon_buf;
	bytes_left = bss_entry->beacon_buf_size;

	/* Process variable IE */
	while (bytes_left >= 2) {
		element_id = *current_ptr;
		element_len = *(current_ptr + 1);
		total_ie_len = element_len + sizeof(struct ieee_types_header);

		if (bytes_left < total_ie_len) {
			mwifiex_dbg(adapter, ERROR,
				    "err: InterpretIE: in processing\t"
				    "IE, bytes left < IE length\n");
			return -EINVAL;
		}
		switch (element_id) {
		case WLAN_EID_SSID:
			if (element_len > IEEE80211_MAX_SSID_LEN)
				return -EINVAL;
			bss_entry->ssid.ssid_len = element_len;
			memcpy(bss_entry->ssid.ssid, (current_ptr + 2),
			       element_len);
			mwifiex_dbg(adapter, INFO,
				    "info: InterpretIE: ssid: %-32s\n",
				    bss_entry->ssid.ssid);
			break;

		case WLAN_EID_SUPP_RATES:
			if (element_len > MWIFIEX_SUPPORTED_RATES)
				return -EINVAL;
			memcpy(bss_entry->data_rates, current_ptr + 2,
			       element_len);
			memcpy(bss_entry->supported_rates, current_ptr + 2,
			       element_len);
			rate_size = element_len;
			found_data_rate_ie = true;
			break;

		case WLAN_EID_FH_PARAMS:
			if (total_ie_len < sizeof(*fh_param_set))
				return -EINVAL;
			fh_param_set =
				(struct ieee_types_fh_param_set *) current_ptr;
			memcpy(&bss_entry->phy_param_set.fh_param_set,
			       fh_param_set,
			       sizeof(struct ieee_types_fh_param_set));
			break;

		case WLAN_EID_DS_PARAMS:
			if (total_ie_len < sizeof(*ds_param_set))
				return -EINVAL;
			ds_param_set =
				(struct ieee_types_ds_param_set *) current_ptr;

			bss_entry->channel = ds_param_set->current_chan;

			memcpy(&bss_entry->phy_param_set.ds_param_set,
			       ds_param_set,
			       sizeof(struct ieee_types_ds_param_set));
			break;

		case WLAN_EID_CF_PARAMS:
			if (total_ie_len < sizeof(*cf_param_set))
				return -EINVAL;
			cf_param_set =
				(struct ieee_types_cf_param_set *) current_ptr;
			memcpy(&bss_entry->ss_param_set.cf_param_set,
			       cf_param_set,
			       sizeof(struct ieee_types_cf_param_set));
			break;

		case WLAN_EID_IBSS_PARAMS:
			if (total_ie_len < sizeof(*ibss_param_set))
				return -EINVAL;
			ibss_param_set =
				(struct ieee_types_ibss_param_set *)
				current_ptr;
			memcpy(&bss_entry->ss_param_set.ibss_param_set,
			       ibss_param_set,
			       sizeof(struct ieee_types_ibss_param_set));
			break;

		case WLAN_EID_ERP_INFO:
			if (!element_len)
				return -EINVAL;
			bss_entry->erp_flags = *(current_ptr + 2);
			break;

		case WLAN_EID_PWR_CONSTRAINT:
			if (!element_len)
				return -EINVAL;
			bss_entry->local_constraint = *(current_ptr + 2);
			bss_entry->sensed_11h = true;
			break;

		case WLAN_EID_CHANNEL_SWITCH:
			bss_entry->chan_sw_ie_present = true;
			/* fall through */
		case WLAN_EID_PWR_CAPABILITY:
		case WLAN_EID_TPC_REPORT:
		case WLAN_EID_QUIET:
			bss_entry->sensed_11h = true;
		    break;

		case WLAN_EID_EXT_SUPP_RATES:
			/*
			 * Only process extended supported rate
			 * if data rate is already found.
			 * Data rate IE should come before
			 * extended supported rate IE
			 */
			if (found_data_rate_ie) {
				if ((element_len + rate_size) >
				    MWIFIEX_SUPPORTED_RATES)
					bytes_to_copy =
						(MWIFIEX_SUPPORTED_RATES -
						 rate_size);
				else
					bytes_to_copy = element_len;

				rate = (u8 *) bss_entry->data_rates;
				rate += rate_size;
				memcpy(rate, current_ptr + 2, bytes_to_copy);

				rate = (u8 *) bss_entry->supported_rates;
				rate += rate_size;
				memcpy(rate, current_ptr + 2, bytes_to_copy);
			}
			break;

		case WLAN_EID_VENDOR_SPECIFIC:
			vendor_ie = (struct ieee_types_vendor_specific *)
					current_ptr;

			/* 802.11 requires at least 3-byte OUI. */
			if (element_len < sizeof(vendor_ie->vend_hdr.oui.oui))
				return -EINVAL;

			/* Not long enough for a match? Skip it. */
			if (element_len < sizeof(wpa_oui))
				break;

			if (!memcmp(&vendor_ie->vend_hdr.oui, wpa_oui,
				    sizeof(wpa_oui))) {
				bss_entry->bcn_wpa_ie =
					(struct ieee_types_vendor_specific *)
					current_ptr;
				bss_entry->wpa_offset = (u16)
					(current_ptr - bss_entry->beacon_buf);
			} else if (!memcmp(&vendor_ie->vend_hdr.oui, wmm_oui,
				    sizeof(wmm_oui))) {
				if (total_ie_len ==
				    sizeof(struct ieee_types_wmm_parameter) ||
				    total_ie_len ==
				    sizeof(struct ieee_types_wmm_info))
					/*
					 * Only accept and copy the WMM IE if
					 * it matches the size expected for the
					 * WMM Info IE or the WMM Parameter IE.
					 */
					memcpy((u8 *) &bss_entry->wmm_ie,
					       current_ptr, total_ie_len);
			}
			break;
		case WLAN_EID_RSN:
			bss_entry->bcn_rsn_ie =
				(struct ieee_types_generic *) current_ptr;
			bss_entry->rsn_offset = (u16) (current_ptr -
							bss_entry->beacon_buf);
			break;
		case WLAN_EID_BSS_AC_ACCESS_DELAY:
			bss_entry->bcn_wapi_ie =
				(struct ieee_types_generic *) current_ptr;
			bss_entry->wapi_offset = (u16) (current_ptr -
							bss_entry->beacon_buf);
			break;
		case WLAN_EID_HT_CAPABILITY:
			bss_entry->bcn_ht_cap = (struct ieee80211_ht_cap *)
					(current_ptr +
					sizeof(struct ieee_types_header));
			bss_entry->ht_cap_offset = (u16) (current_ptr +
					sizeof(struct ieee_types_header) -
					bss_entry->beacon_buf);
			break;
		case WLAN_EID_HT_OPERATION:
			bss_entry->bcn_ht_oper =
				(struct ieee80211_ht_operation *)(current_ptr +
					sizeof(struct ieee_types_header));
			bss_entry->ht_info_offset = (u16) (current_ptr +
					sizeof(struct ieee_types_header) -
					bss_entry->beacon_buf);
			break;
		case WLAN_EID_VHT_CAPABILITY:
			bss_entry->disable_11ac = false;
			bss_entry->bcn_vht_cap =
				(void *)(current_ptr +
					 sizeof(struct ieee_types_header));
			bss_entry->vht_cap_offset =
					(u16)((u8 *)bss_entry->bcn_vht_cap -
					      bss_entry->beacon_buf);
			break;
		case WLAN_EID_VHT_OPERATION:
			bss_entry->bcn_vht_oper =
				(void *)(current_ptr +
					 sizeof(struct ieee_types_header));
			bss_entry->vht_info_offset =
					(u16)((u8 *)bss_entry->bcn_vht_oper -
					      bss_entry->beacon_buf);
			break;
		case WLAN_EID_BSS_COEX_2040:
			bss_entry->bcn_bss_co_2040 = current_ptr;
			bss_entry->bss_co_2040_offset =
				(u16) (current_ptr - bss_entry->beacon_buf);
			break;
		case WLAN_EID_EXT_CAPABILITY:
			bss_entry->bcn_ext_cap = current_ptr;
			bss_entry->ext_cap_offset =
				(u16) (current_ptr - bss_entry->beacon_buf);
			break;
		case WLAN_EID_OPMODE_NOTIF:
			bss_entry->oper_mode = (void *)current_ptr;
			bss_entry->oper_mode_offset =
					(u16)((u8 *)bss_entry->oper_mode -
					      bss_entry->beacon_buf);
			break;
		default:
			break;
		}

		current_ptr += total_ie_len;
		bytes_left -= total_ie_len;

	}	/* while (bytes_left > 2) */
	return ret;
}