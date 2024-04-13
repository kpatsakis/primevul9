static void wmi_evt_rx_mgmt(struct wil6210_priv *wil, int id, void *d, int len)
{
	struct wmi_rx_mgmt_packet_event *data = d;
	struct wiphy *wiphy = wil_to_wiphy(wil);
	struct ieee80211_mgmt *rx_mgmt_frame =
			(struct ieee80211_mgmt *)data->payload;
	int flen = len - offsetof(struct wmi_rx_mgmt_packet_event, payload);
	int ch_no;
	u32 freq;
	struct ieee80211_channel *channel;
	s32 signal;
	__le16 fc;
	u32 d_len;
	u16 d_status;

	if (flen < 0) {
		wil_err(wil, "MGMT Rx: short event, len %d\n", len);
		return;
	}

	d_len = le32_to_cpu(data->info.len);
	if (d_len != flen) {
		wil_err(wil,
			"MGMT Rx: length mismatch, d_len %d should be %d\n",
			d_len, flen);
		return;
	}

	ch_no = data->info.channel + 1;
	freq = ieee80211_channel_to_frequency(ch_no, NL80211_BAND_60GHZ);
	channel = ieee80211_get_channel(wiphy, freq);
	if (test_bit(WMI_FW_CAPABILITY_RSSI_REPORTING, wil->fw_capabilities))
		signal = 100 * data->info.rssi;
	else
		signal = data->info.sqi;
	d_status = le16_to_cpu(data->info.status);
	fc = rx_mgmt_frame->frame_control;

	wil_dbg_wmi(wil, "MGMT Rx: channel %d MCS %d RSSI %d SQI %d%%\n",
		    data->info.channel, data->info.mcs, data->info.rssi,
		    data->info.sqi);
	wil_dbg_wmi(wil, "status 0x%04x len %d fc 0x%04x\n", d_status, d_len,
		    le16_to_cpu(fc));
	wil_dbg_wmi(wil, "qid %d mid %d cid %d\n",
		    data->info.qid, data->info.mid, data->info.cid);
	wil_hex_dump_wmi("MGMT Rx ", DUMP_PREFIX_OFFSET, 16, 1, rx_mgmt_frame,
			 d_len, true);

	if (!channel) {
		wil_err(wil, "Frame on unsupported channel\n");
		return;
	}

	if (ieee80211_is_beacon(fc) || ieee80211_is_probe_resp(fc)) {
		struct cfg80211_bss *bss;
		u64 tsf = le64_to_cpu(rx_mgmt_frame->u.beacon.timestamp);
		u16 cap = le16_to_cpu(rx_mgmt_frame->u.beacon.capab_info);
		u16 bi = le16_to_cpu(rx_mgmt_frame->u.beacon.beacon_int);
		const u8 *ie_buf = rx_mgmt_frame->u.beacon.variable;
		size_t ie_len = d_len - offsetof(struct ieee80211_mgmt,
						 u.beacon.variable);
		wil_dbg_wmi(wil, "Capability info : 0x%04x\n", cap);
		wil_dbg_wmi(wil, "TSF : 0x%016llx\n", tsf);
		wil_dbg_wmi(wil, "Beacon interval : %d\n", bi);
		wil_hex_dump_wmi("IE ", DUMP_PREFIX_OFFSET, 16, 1, ie_buf,
				 ie_len, true);

		wil_dbg_wmi(wil, "Capability info : 0x%04x\n", cap);

		bss = cfg80211_inform_bss_frame(wiphy, channel, rx_mgmt_frame,
						d_len, signal, GFP_KERNEL);
		if (bss) {
			wil_dbg_wmi(wil, "Added BSS %pM\n",
				    rx_mgmt_frame->bssid);
			cfg80211_put_bss(wiphy, bss);
		} else {
			wil_err(wil, "cfg80211_inform_bss_frame() failed\n");
		}
	} else {
		mutex_lock(&wil->p2p_wdev_mutex);
		cfg80211_rx_mgmt(wil->radio_wdev, freq, signal,
				 (void *)rx_mgmt_frame, d_len, 0);
		mutex_unlock(&wil->p2p_wdev_mutex);
	}
}