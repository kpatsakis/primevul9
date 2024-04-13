static int ath6kl_wmi_connect_event_rx(struct wmi *wmi, u8 *datap, int len,
				       struct ath6kl_vif *vif)
{
	struct wmi_connect_event *ev;
	u8 *pie, *peie;

	if (len < sizeof(struct wmi_connect_event))
		return -EINVAL;

	ev = (struct wmi_connect_event *) datap;

	if (vif->nw_type == AP_NETWORK) {
		/* AP mode start/STA connected event */
		struct net_device *dev = vif->ndev;
		if (memcmp(dev->dev_addr, ev->u.ap_bss.bssid, ETH_ALEN) == 0) {
			ath6kl_dbg(ATH6KL_DBG_WMI,
				   "%s: freq %d bssid %pM (AP started)\n",
				   __func__, le16_to_cpu(ev->u.ap_bss.ch),
				   ev->u.ap_bss.bssid);
			ath6kl_connect_ap_mode_bss(
				vif, le16_to_cpu(ev->u.ap_bss.ch));
		} else {
			ath6kl_dbg(ATH6KL_DBG_WMI,
				   "%s: aid %u mac_addr %pM auth=%u keymgmt=%u cipher=%u apsd_info=%u (STA connected)\n",
				   __func__, ev->u.ap_sta.aid,
				   ev->u.ap_sta.mac_addr,
				   ev->u.ap_sta.auth,
				   ev->u.ap_sta.keymgmt,
				   le16_to_cpu(ev->u.ap_sta.cipher),
				   ev->u.ap_sta.apsd_info);

			ath6kl_connect_ap_mode_sta(
				vif, ev->u.ap_sta.aid, ev->u.ap_sta.mac_addr,
				ev->u.ap_sta.keymgmt,
				le16_to_cpu(ev->u.ap_sta.cipher),
				ev->u.ap_sta.auth, ev->assoc_req_len,
				ev->assoc_info + ev->beacon_ie_len,
				ev->u.ap_sta.apsd_info);
		}
		return 0;
	}

	/* STA/IBSS mode connection event */

	ath6kl_dbg(ATH6KL_DBG_WMI,
		   "wmi event connect freq %d bssid %pM listen_intvl %d beacon_intvl %d type %d\n",
		   le16_to_cpu(ev->u.sta.ch), ev->u.sta.bssid,
		   le16_to_cpu(ev->u.sta.listen_intvl),
		   le16_to_cpu(ev->u.sta.beacon_intvl),
		   le32_to_cpu(ev->u.sta.nw_type));

	/* Start of assoc rsp IEs */
	pie = ev->assoc_info + ev->beacon_ie_len +
	      ev->assoc_req_len + (sizeof(u16) * 3); /* capinfo, status, aid */

	/* End of assoc rsp IEs */
	peie = ev->assoc_info + ev->beacon_ie_len + ev->assoc_req_len +
	    ev->assoc_resp_len;

	while (pie < peie) {
		switch (*pie) {
		case WLAN_EID_VENDOR_SPECIFIC:
			if (pie[1] > 3 && pie[2] == 0x00 && pie[3] == 0x50 &&
			    pie[4] == 0xf2 && pie[5] == WMM_OUI_TYPE) {
				/* WMM OUT (00:50:F2) */
				if (pie[1] > 5 &&
				    pie[6] == WMM_PARAM_OUI_SUBTYPE)
					wmi->is_wmm_enabled = true;
			}
			break;
		}

		if (wmi->is_wmm_enabled)
			break;

		pie += pie[1] + 2;
	}

	ath6kl_connect_event(vif, le16_to_cpu(ev->u.sta.ch),
			     ev->u.sta.bssid,
			     le16_to_cpu(ev->u.sta.listen_intvl),
			     le16_to_cpu(ev->u.sta.beacon_intvl),
			     le32_to_cpu(ev->u.sta.nw_type),
			     ev->beacon_ie_len, ev->assoc_req_len,
			     ev->assoc_resp_len, ev->assoc_info);

	return 0;
}