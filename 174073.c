mwifiex_parse_single_response_buf(struct mwifiex_private *priv, u8 **bss_info,
				  u32 *bytes_left, u64 fw_tsf, u8 *radio_type,
				  bool ext_scan, s32 rssi_val)
{
	struct mwifiex_adapter *adapter = priv->adapter;
	struct mwifiex_chan_freq_power *cfp;
	struct cfg80211_bss *bss;
	u8 bssid[ETH_ALEN];
	s32 rssi;
	const u8 *ie_buf;
	size_t ie_len;
	u16 channel = 0;
	u16 beacon_size = 0;
	u32 curr_bcn_bytes;
	u32 freq;
	u16 beacon_period;
	u16 cap_info_bitmap;
	u8 *current_ptr;
	u64 timestamp;
	struct mwifiex_fixed_bcn_param *bcn_param;
	struct mwifiex_bss_priv *bss_priv;

	if (*bytes_left >= sizeof(beacon_size)) {
		/* Extract & convert beacon size from command buffer */
		beacon_size = get_unaligned_le16((*bss_info));
		*bytes_left -= sizeof(beacon_size);
		*bss_info += sizeof(beacon_size);
	}

	if (!beacon_size || beacon_size > *bytes_left) {
		*bss_info += *bytes_left;
		*bytes_left = 0;
		return -EFAULT;
	}

	/* Initialize the current working beacon pointer for this BSS
	 * iteration
	 */
	current_ptr = *bss_info;

	/* Advance the return beacon pointer past the current beacon */
	*bss_info += beacon_size;
	*bytes_left -= beacon_size;

	curr_bcn_bytes = beacon_size;

	/* First 5 fields are bssid, RSSI(for legacy scan only),
	 * time stamp, beacon interval, and capability information
	 */
	if (curr_bcn_bytes < ETH_ALEN + sizeof(u8) +
	    sizeof(struct mwifiex_fixed_bcn_param)) {
		mwifiex_dbg(adapter, ERROR,
			    "InterpretIE: not enough bytes left\n");
		return -EFAULT;
	}

	memcpy(bssid, current_ptr, ETH_ALEN);
	current_ptr += ETH_ALEN;
	curr_bcn_bytes -= ETH_ALEN;

	if (!ext_scan) {
		rssi = (s32) *current_ptr;
		rssi = (-rssi) * 100;		/* Convert dBm to mBm */
		current_ptr += sizeof(u8);
		curr_bcn_bytes -= sizeof(u8);
		mwifiex_dbg(adapter, INFO,
			    "info: InterpretIE: RSSI=%d\n", rssi);
	} else {
		rssi = rssi_val;
	}

	bcn_param = (struct mwifiex_fixed_bcn_param *)current_ptr;
	current_ptr += sizeof(*bcn_param);
	curr_bcn_bytes -= sizeof(*bcn_param);

	timestamp = le64_to_cpu(bcn_param->timestamp);
	beacon_period = le16_to_cpu(bcn_param->beacon_period);

	cap_info_bitmap = le16_to_cpu(bcn_param->cap_info_bitmap);
	mwifiex_dbg(adapter, INFO,
		    "info: InterpretIE: capabilities=0x%X\n",
		    cap_info_bitmap);

	/* Rest of the current buffer are IE's */
	ie_buf = current_ptr;
	ie_len = curr_bcn_bytes;
	mwifiex_dbg(adapter, INFO,
		    "info: InterpretIE: IELength for this AP = %d\n",
		    curr_bcn_bytes);

	while (curr_bcn_bytes >= sizeof(struct ieee_types_header)) {
		u8 element_id, element_len;

		element_id = *current_ptr;
		element_len = *(current_ptr + 1);
		if (curr_bcn_bytes < element_len +
				sizeof(struct ieee_types_header)) {
			mwifiex_dbg(adapter, ERROR,
				    "%s: bytes left < IE length\n", __func__);
			return -EFAULT;
		}
		if (element_id == WLAN_EID_DS_PARAMS) {
			channel = *(current_ptr +
				    sizeof(struct ieee_types_header));
			break;
		}

		current_ptr += element_len + sizeof(struct ieee_types_header);
		curr_bcn_bytes -= element_len +
					sizeof(struct ieee_types_header);
	}

	if (channel) {
		struct ieee80211_channel *chan;
		u8 band;

		/* Skip entry if on csa closed channel */
		if (channel == priv->csa_chan) {
			mwifiex_dbg(adapter, WARN,
				    "Dropping entry on csa closed channel\n");
			return 0;
		}

		band = BAND_G;
		if (radio_type)
			band = mwifiex_radio_type_to_band(*radio_type &
							  (BIT(0) | BIT(1)));

		cfp = mwifiex_get_cfp(priv, band, channel, 0);

		freq = cfp ? cfp->freq : 0;

		chan = ieee80211_get_channel(priv->wdev.wiphy, freq);

		if (chan && !(chan->flags & IEEE80211_CHAN_DISABLED)) {
			bss = cfg80211_inform_bss(priv->wdev.wiphy,
					    chan, CFG80211_BSS_FTYPE_UNKNOWN,
					    bssid, timestamp,
					    cap_info_bitmap, beacon_period,
					    ie_buf, ie_len, rssi, GFP_KERNEL);
			if (bss) {
				bss_priv = (struct mwifiex_bss_priv *)bss->priv;
				bss_priv->band = band;
				bss_priv->fw_tsf = fw_tsf;
				if (priv->media_connected &&
				    !memcmp(bssid, priv->curr_bss_params.
					    bss_descriptor.mac_address,
					    ETH_ALEN))
					mwifiex_update_curr_bss_params(priv,
								       bss);

				if ((chan->flags & IEEE80211_CHAN_RADAR) ||
				    (chan->flags & IEEE80211_CHAN_NO_IR)) {
					mwifiex_dbg(adapter, INFO,
						    "radar or passive channel %d\n",
						    channel);
					mwifiex_save_hidden_ssid_channels(priv,
									  bss);
				}

				cfg80211_put_bss(priv->wdev.wiphy, bss);
			}
		}
	} else {
		mwifiex_dbg(adapter, WARN, "missing BSS channel IE\n");
	}

	return 0;
}