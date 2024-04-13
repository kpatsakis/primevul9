static void wmi_evt_connect(struct wil6210_priv *wil, int id, void *d, int len)
{
	struct net_device *ndev = wil_to_ndev(wil);
	struct wireless_dev *wdev = wil->wdev;
	struct wmi_connect_event *evt = d;
	int ch; /* channel number */
	struct station_info sinfo;
	u8 *assoc_req_ie, *assoc_resp_ie;
	size_t assoc_req_ielen, assoc_resp_ielen;
	/* capinfo(u16) + listen_interval(u16) + IEs */
	const size_t assoc_req_ie_offset = sizeof(u16) * 2;
	/* capinfo(u16) + status_code(u16) + associd(u16) + IEs */
	const size_t assoc_resp_ie_offset = sizeof(u16) * 3;
	int rc;

	if (len < sizeof(*evt)) {
		wil_err(wil, "Connect event too short : %d bytes\n", len);
		return;
	}
	if (len != sizeof(*evt) + evt->beacon_ie_len + evt->assoc_req_len +
		   evt->assoc_resp_len) {
		wil_err(wil,
			"Connect event corrupted : %d != %d + %d + %d + %d\n",
			len, (int)sizeof(*evt), evt->beacon_ie_len,
			evt->assoc_req_len, evt->assoc_resp_len);
		return;
	}
	if (evt->cid >= WIL6210_MAX_CID) {
		wil_err(wil, "Connect CID invalid : %d\n", evt->cid);
		return;
	}

	ch = evt->channel + 1;
	wil_info(wil, "Connect %pM channel [%d] cid %d aid %d\n",
		 evt->bssid, ch, evt->cid, evt->aid);
	wil_hex_dump_wmi("connect AI : ", DUMP_PREFIX_OFFSET, 16, 1,
			 evt->assoc_info, len - sizeof(*evt), true);

	/* figure out IE's */
	assoc_req_ie = &evt->assoc_info[evt->beacon_ie_len +
					assoc_req_ie_offset];
	assoc_req_ielen = evt->assoc_req_len - assoc_req_ie_offset;
	if (evt->assoc_req_len <= assoc_req_ie_offset) {
		assoc_req_ie = NULL;
		assoc_req_ielen = 0;
	}

	assoc_resp_ie = &evt->assoc_info[evt->beacon_ie_len +
					 evt->assoc_req_len +
					 assoc_resp_ie_offset];
	assoc_resp_ielen = evt->assoc_resp_len - assoc_resp_ie_offset;
	if (evt->assoc_resp_len <= assoc_resp_ie_offset) {
		assoc_resp_ie = NULL;
		assoc_resp_ielen = 0;
	}

	if (test_bit(wil_status_resetting, wil->status) ||
	    !test_bit(wil_status_fwready, wil->status)) {
		wil_err(wil, "status_resetting, cancel connect event, CID %d\n",
			evt->cid);
		/* no need for cleanup, wil_reset will do that */
		return;
	}

	mutex_lock(&wil->mutex);

	if ((wdev->iftype == NL80211_IFTYPE_STATION) ||
	    (wdev->iftype == NL80211_IFTYPE_P2P_CLIENT)) {
		if (!test_bit(wil_status_fwconnecting, wil->status)) {
			wil_err(wil, "Not in connecting state\n");
			mutex_unlock(&wil->mutex);
			return;
		}
		del_timer_sync(&wil->connect_timer);
	} else if ((wdev->iftype == NL80211_IFTYPE_AP) ||
		   (wdev->iftype == NL80211_IFTYPE_P2P_GO)) {
		if (wil->sta[evt->cid].status != wil_sta_unused) {
			wil_err(wil, "AP: Invalid status %d for CID %d\n",
				wil->sta[evt->cid].status, evt->cid);
			mutex_unlock(&wil->mutex);
			return;
		}
	}

	/* FIXME FW can transmit only ucast frames to peer */
	/* FIXME real ring_id instead of hard coded 0 */
	ether_addr_copy(wil->sta[evt->cid].addr, evt->bssid);
	wil->sta[evt->cid].status = wil_sta_conn_pending;

	rc = wil_tx_init(wil, evt->cid);
	if (rc) {
		wil_err(wil, "config tx vring failed for CID %d, rc (%d)\n",
			evt->cid, rc);
		wmi_disconnect_sta(wil, wil->sta[evt->cid].addr,
				   WLAN_REASON_UNSPECIFIED, false, false);
	} else {
		wil_info(wil, "successful connection to CID %d\n", evt->cid);
	}

	if ((wdev->iftype == NL80211_IFTYPE_STATION) ||
	    (wdev->iftype == NL80211_IFTYPE_P2P_CLIENT)) {
		if (rc) {
			netif_carrier_off(ndev);
			wil6210_bus_request(wil, WIL_DEFAULT_BUS_REQUEST_KBPS);
			wil_err(wil, "cfg80211_connect_result with failure\n");
			cfg80211_connect_result(ndev, evt->bssid, NULL, 0,
						NULL, 0,
						WLAN_STATUS_UNSPECIFIED_FAILURE,
						GFP_KERNEL);
			goto out;
		} else {
			struct wiphy *wiphy = wil_to_wiphy(wil);

			cfg80211_ref_bss(wiphy, wil->bss);
			cfg80211_connect_bss(ndev, evt->bssid, wil->bss,
					     assoc_req_ie, assoc_req_ielen,
					     assoc_resp_ie, assoc_resp_ielen,
					     WLAN_STATUS_SUCCESS, GFP_KERNEL,
					     NL80211_TIMEOUT_UNSPECIFIED);
		}
		wil->bss = NULL;
	} else if ((wdev->iftype == NL80211_IFTYPE_AP) ||
		   (wdev->iftype == NL80211_IFTYPE_P2P_GO)) {
		if (rc) {
			if (disable_ap_sme)
				/* notify new_sta has failed */
				cfg80211_del_sta(ndev, evt->bssid, GFP_KERNEL);
			goto out;
		}

		memset(&sinfo, 0, sizeof(sinfo));

		sinfo.generation = wil->sinfo_gen++;

		if (assoc_req_ie) {
			sinfo.assoc_req_ies = assoc_req_ie;
			sinfo.assoc_req_ies_len = assoc_req_ielen;
		}

		cfg80211_new_sta(ndev, evt->bssid, &sinfo, GFP_KERNEL);
	} else {
		wil_err(wil, "unhandled iftype %d for CID %d\n", wdev->iftype,
			evt->cid);
		goto out;
	}

	wil->sta[evt->cid].status = wil_sta_connected;
	wil->sta[evt->cid].aid = evt->aid;
	set_bit(wil_status_fwconnected, wil->status);
	wil_update_net_queues_bh(wil, NULL, false);

out:
	if (rc)
		wil->sta[evt->cid].status = wil_sta_unused;
	clear_bit(wil_status_fwconnecting, wil->status);
	mutex_unlock(&wil->mutex);
}