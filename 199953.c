void ieee80211_rx_mgt(struct ieee80211_device *ieee,
		      struct ieee80211_hdr_4addr *header,
		      struct ieee80211_rx_stats *stats)
{
	switch (WLAN_FC_GET_STYPE(le16_to_cpu(header->frame_ctl))) {
	case IEEE80211_STYPE_ASSOC_RESP:
		IEEE80211_DEBUG_MGMT("received ASSOCIATION RESPONSE (%d)\n",
				     WLAN_FC_GET_STYPE(le16_to_cpu
						       (header->frame_ctl)));
		ieee80211_handle_assoc_resp(ieee,
					    (struct ieee80211_assoc_response *)
					    header, stats);
		break;

	case IEEE80211_STYPE_REASSOC_RESP:
		IEEE80211_DEBUG_MGMT("received REASSOCIATION RESPONSE (%d)\n",
				     WLAN_FC_GET_STYPE(le16_to_cpu
						       (header->frame_ctl)));
		break;

	case IEEE80211_STYPE_PROBE_REQ:
		IEEE80211_DEBUG_MGMT("received auth (%d)\n",
				     WLAN_FC_GET_STYPE(le16_to_cpu
						       (header->frame_ctl)));

		if (ieee->handle_probe_request != NULL)
			ieee->handle_probe_request(ieee->dev,
						   (struct
						    ieee80211_probe_request *)
						   header, stats);
		break;

	case IEEE80211_STYPE_PROBE_RESP:
		IEEE80211_DEBUG_MGMT("received PROBE RESPONSE (%d)\n",
				     WLAN_FC_GET_STYPE(le16_to_cpu
						       (header->frame_ctl)));
		IEEE80211_DEBUG_SCAN("Probe response\n");
		ieee80211_process_probe_response(ieee,
						 (struct
						  ieee80211_probe_response *)
						 header, stats);
		break;

	case IEEE80211_STYPE_BEACON:
		IEEE80211_DEBUG_MGMT("received BEACON (%d)\n",
				     WLAN_FC_GET_STYPE(le16_to_cpu
						       (header->frame_ctl)));
		IEEE80211_DEBUG_SCAN("Beacon\n");
		ieee80211_process_probe_response(ieee,
						 (struct
						  ieee80211_probe_response *)
						 header, stats);
		break;
	case IEEE80211_STYPE_AUTH:

		IEEE80211_DEBUG_MGMT("received auth (%d)\n",
				     WLAN_FC_GET_STYPE(le16_to_cpu
						       (header->frame_ctl)));

		if (ieee->handle_auth != NULL)
			ieee->handle_auth(ieee->dev,
					  (struct ieee80211_auth *)header);
		break;

	case IEEE80211_STYPE_DISASSOC:
		if (ieee->handle_disassoc != NULL)
			ieee->handle_disassoc(ieee->dev,
					      (struct ieee80211_disassoc *)
					      header);
		break;

	case IEEE80211_STYPE_ACTION:
		IEEE80211_DEBUG_MGMT("ACTION\n");
		if (ieee->handle_action)
			ieee->handle_action(ieee->dev,
					    (struct ieee80211_action *)
					    header, stats);
		break;

	case IEEE80211_STYPE_REASSOC_REQ:
		IEEE80211_DEBUG_MGMT("received reassoc (%d)\n",
				     WLAN_FC_GET_STYPE(le16_to_cpu
						       (header->frame_ctl)));

		IEEE80211_DEBUG_MGMT("%s: IEEE80211_REASSOC_REQ received\n",
				     ieee->dev->name);
		if (ieee->handle_reassoc_request != NULL)
			ieee->handle_reassoc_request(ieee->dev,
						    (struct ieee80211_reassoc_request *)
						     header);
		break;

	case IEEE80211_STYPE_ASSOC_REQ:
		IEEE80211_DEBUG_MGMT("received assoc (%d)\n",
				     WLAN_FC_GET_STYPE(le16_to_cpu
						       (header->frame_ctl)));

		IEEE80211_DEBUG_MGMT("%s: IEEE80211_ASSOC_REQ received\n",
				     ieee->dev->name);
		if (ieee->handle_assoc_request != NULL)
			ieee->handle_assoc_request(ieee->dev);
		break;

	case IEEE80211_STYPE_DEAUTH:
		IEEE80211_DEBUG_MGMT("DEAUTH\n");
		if (ieee->handle_deauth != NULL)
			ieee->handle_deauth(ieee->dev,
					    (struct ieee80211_deauth *)
					    header);
		break;
	default:
		IEEE80211_DEBUG_MGMT("received UNKNOWN (%d)\n",
				     WLAN_FC_GET_STYPE(le16_to_cpu
						       (header->frame_ctl)));
		IEEE80211_DEBUG_MGMT("%s: Unknown management packet: %d\n",
				     ieee->dev->name,
				     WLAN_FC_GET_STYPE(le16_to_cpu
						       (header->frame_ctl)));
		break;
	}
}