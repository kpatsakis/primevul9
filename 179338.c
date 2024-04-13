static char *translate_scan(struct adapter *padapter,
			    struct iw_request_info *info,
			    struct wlan_network *pnetwork,
			    char *start, char *stop)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct iw_event iwe;
	u16 cap;
	__le16 le_tmp;
	u32 ht_ielen = 0;
	char custom[MAX_CUSTOM_LEN];
	char *p;
	u16 max_rate = 0, rate, ht_cap = false;
	u32 i = 0;
	u8 bw_40MHz = 0, short_GI = 0;
	u16 mcs_rate = 0;
	u8 ss, sq;

	/*  AP MAC address  */
	iwe.cmd = SIOCGIWAP;
	iwe.u.ap_addr.sa_family = ARPHRD_ETHER;

	memcpy(iwe.u.ap_addr.sa_data, pnetwork->network.MacAddress, ETH_ALEN);
	start = iwe_stream_add_event(info, start, stop, &iwe, IW_EV_ADDR_LEN);

	/* Add the ESSID */
	iwe.cmd = SIOCGIWESSID;
	iwe.u.data.flags = 1;
	iwe.u.data.length = min_t(u16, pnetwork->network.ssid.ssid_length, 32);
	start = iwe_stream_add_point(info, start, stop, &iwe, pnetwork->network.ssid.ssid);

	/* parsing HT_CAP_IE */
	p = rtw_get_ie(&pnetwork->network.ies[12], WLAN_EID_HT_CAPABILITY, &ht_ielen, pnetwork->network.ie_length - 12);

	if (p && ht_ielen > 0) {
		struct ieee80211_ht_cap *pht_capie;

		ht_cap = true;

		pht_capie = (struct ieee80211_ht_cap *)(p + 2);
		memcpy(&mcs_rate, pht_capie->mcs.rx_mask, 2);
		bw_40MHz = !!(le16_to_cpu(pht_capie->cap_info) &
			      IEEE80211_HT_CAP_SUP_WIDTH_20_40);
		short_GI = !!(le16_to_cpu(pht_capie->cap_info) &
			      (IEEE80211_HT_CAP_SGI_20 |
			       IEEE80211_HT_CAP_SGI_40));
	}

	/* Add the protocol name */
	iwe.cmd = SIOCGIWNAME;
	if ((rtw_is_cckratesonly_included((u8 *)&pnetwork->network.SupportedRates))) {
		if (ht_cap)
			snprintf(iwe.u.name, IFNAMSIZ, "IEEE 802.11bn");
		else
			snprintf(iwe.u.name, IFNAMSIZ, "IEEE 802.11b");
	} else if ((rtw_is_cckrates_included((u8 *)&pnetwork->network.SupportedRates))) {
		if (ht_cap)
			snprintf(iwe.u.name, IFNAMSIZ, "IEEE 802.11bgn");
		else
			snprintf(iwe.u.name, IFNAMSIZ, "IEEE 802.11bg");
	} else {
		if (ht_cap)
			snprintf(iwe.u.name, IFNAMSIZ, "IEEE 802.11gn");
		else
			snprintf(iwe.u.name, IFNAMSIZ, "IEEE 802.11g");
	}

	start = iwe_stream_add_event(info, start, stop, &iwe, IW_EV_CHAR_LEN);

	  /* Add mode */
	iwe.cmd = SIOCGIWMODE;
	memcpy(&le_tmp, rtw_get_capability_from_ie(pnetwork->network.ies), 2);

	cap = le16_to_cpu(le_tmp);

	if (!WLAN_CAPABILITY_IS_STA_BSS(cap)) {
		if (cap & WLAN_CAPABILITY_ESS)
			iwe.u.mode = IW_MODE_MASTER;
		else
			iwe.u.mode = IW_MODE_ADHOC;

		start = iwe_stream_add_event(info, start, stop, &iwe, IW_EV_UINT_LEN);
	}

	if (pnetwork->network.Configuration.DSConfig < 1)
		pnetwork->network.Configuration.DSConfig = 1;

	 /* Add frequency/channel */
	iwe.cmd = SIOCGIWFREQ;
	iwe.u.freq.m = rtw_ch2freq(pnetwork->network.Configuration.DSConfig) * 100000;
	iwe.u.freq.e = 1;
	iwe.u.freq.i = pnetwork->network.Configuration.DSConfig;
	start = iwe_stream_add_event(info, start, stop, &iwe, IW_EV_FREQ_LEN);

	/* Add encryption capability */
	iwe.cmd = SIOCGIWENCODE;
	if (cap & WLAN_CAPABILITY_PRIVACY)
		iwe.u.data.flags = IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
	else
		iwe.u.data.flags = IW_ENCODE_DISABLED;
	iwe.u.data.length = 0;
	start = iwe_stream_add_point(info, start, stop, &iwe, pnetwork->network.ssid.ssid);

	/*Add basic and extended rates */
	max_rate = 0;
	p = custom;
	p += scnprintf(p, MAX_CUSTOM_LEN - (p - custom), " Rates (Mb/s): ");
	while (pnetwork->network.SupportedRates[i] != 0) {
		rate = pnetwork->network.SupportedRates[i] & 0x7F;
		if (rate > max_rate)
			max_rate = rate;
		p += scnprintf(p, MAX_CUSTOM_LEN - (p - custom),
			      "%d%s ", rate >> 1, (rate & 1) ? ".5" : "");
		i++;
	}

	if (ht_cap) {
		if (mcs_rate & 0x8000)/* MCS15 */
			max_rate = (bw_40MHz) ? ((short_GI) ? 300 : 270) : ((short_GI) ? 144 : 130);
		else if (mcs_rate & 0x0080)/* MCS7 */
			;
		else/* default MCS7 */
			max_rate = (bw_40MHz) ? ((short_GI) ? 150 : 135) : ((short_GI) ? 72 : 65);

		max_rate *= 2; /* Mbps/2; */
	}

	iwe.cmd = SIOCGIWRATE;
	iwe.u.bitrate.fixed = 0;
	iwe.u.bitrate.disabled = 0;
	iwe.u.bitrate.value = max_rate * 500000;
	start = iwe_stream_add_event(info, start, stop, &iwe, IW_EV_PARAM_LEN);

	/* parsing WPA/WPA2 IE */
	{
		u8 *buf;
		u8 wpa_ie[255], rsn_ie[255];
		u16 wpa_len = 0, rsn_len = 0;
		u8 *p;

		buf = kzalloc(MAX_WPA_IE_LEN, GFP_ATOMIC);
		if (!buf)
			return start;

		rtw_get_sec_ie(pnetwork->network.ies, pnetwork->network.ie_length, rsn_ie, &rsn_len, wpa_ie, &wpa_len);
		RT_TRACE(_module_rtl871x_mlme_c_, _drv_info_, ("rtw_wx_get_scan: ssid =%s\n", pnetwork->network.ssid.ssid));
		RT_TRACE(_module_rtl871x_mlme_c_, _drv_info_, ("rtw_wx_get_scan: wpa_len =%d rsn_len =%d\n", wpa_len, rsn_len));

		if (wpa_len > 0) {
			p = buf;
			p += sprintf(p, "wpa_ie=");
			for (i = 0; i < wpa_len; i++)
				p += sprintf(p, "%02x", wpa_ie[i]);

			memset(&iwe, 0, sizeof(iwe));
			iwe.cmd = IWEVCUSTOM;
			iwe.u.data.length = strlen(buf);
			start = iwe_stream_add_point(info, start, stop, &iwe, buf);

			memset(&iwe, 0, sizeof(iwe));
			iwe.cmd = IWEVGENIE;
			iwe.u.data.length = wpa_len;
			start = iwe_stream_add_point(info, start, stop, &iwe, wpa_ie);
		}
		if (rsn_len > 0) {
			p = buf;
			p += sprintf(p, "rsn_ie=");
			for (i = 0; i < rsn_len; i++)
				p += sprintf(p, "%02x", rsn_ie[i]);
			memset(&iwe, 0, sizeof(iwe));
			iwe.cmd = IWEVCUSTOM;
			iwe.u.data.length = strlen(buf);
			start = iwe_stream_add_point(info, start, stop, &iwe, buf);

			memset(&iwe, 0, sizeof(iwe));
			iwe.cmd = IWEVGENIE;
			iwe.u.data.length = rsn_len;
			start = iwe_stream_add_point(info, start, stop, &iwe, rsn_ie);
		}
		kfree(buf);
	}

	{/* parsing WPS IE */
		uint cnt = 0, total_ielen;
		u8 *wpsie_ptr = NULL;
		uint wps_ielen = 0;
		u8 *ie_ptr = pnetwork->network.ies + _FIXED_IE_LENGTH_;

		total_ielen = pnetwork->network.ie_length - _FIXED_IE_LENGTH_;

		while (cnt < total_ielen) {
			if (rtw_is_wps_ie(&ie_ptr[cnt], &wps_ielen) && (wps_ielen > 2)) {
				wpsie_ptr = &ie_ptr[cnt];
				iwe.cmd = IWEVGENIE;
				iwe.u.data.length = (u16)wps_ielen;
				start = iwe_stream_add_point(info, start, stop, &iwe, wpsie_ptr);
			}
			cnt += ie_ptr[cnt + 1] + 2; /* goto next */
		}
	}

	/* Add quality statistics */
	iwe.cmd = IWEVQUAL;
	iwe.u.qual.updated = IW_QUAL_QUAL_UPDATED | IW_QUAL_LEVEL_UPDATED | IW_QUAL_NOISE_INVALID;

	if (check_fwstate(pmlmepriv, _FW_LINKED) &&
	    is_same_network(&pmlmepriv->cur_network.network, &pnetwork->network)) {
		ss = padapter->recvpriv.signal_strength;
		sq = padapter->recvpriv.signal_qual;
	} else {
		ss = pnetwork->network.PhyInfo.SignalStrength;
		sq = pnetwork->network.PhyInfo.SignalQuality;
	}

	iwe.u.qual.level = (u8)ss;
	iwe.u.qual.qual = (u8)sq;   /*  signal quality */
	iwe.u.qual.noise = 0; /*  noise level */
	start = iwe_stream_add_event(info, start, stop, &iwe, IW_EV_QUAL_LEN);
	return start;
}