static int ieee80211_handle_assoc_resp(struct ieee80211_device *ieee, struct ieee80211_assoc_response
				       *frame, struct ieee80211_rx_stats *stats)
{
	struct ieee80211_network network_resp = {
		.ibss_dfs = NULL,
	};
	struct ieee80211_network *network = &network_resp;
	struct net_device *dev = ieee->dev;

	network->flags = 0;
	network->qos_data.active = 0;
	network->qos_data.supported = 0;
	network->qos_data.param_count = 0;
	network->qos_data.old_param_count = 0;

	//network->atim_window = le16_to_cpu(frame->aid) & (0x3FFF);
	network->atim_window = le16_to_cpu(frame->aid);
	network->listen_interval = le16_to_cpu(frame->status);
	memcpy(network->bssid, frame->header.addr3, ETH_ALEN);
	network->capability = le16_to_cpu(frame->capability);
	network->last_scanned = jiffies;
	network->rates_len = network->rates_ex_len = 0;
	network->last_associate = 0;
	network->ssid_len = 0;
	network->erp_value =
	    (network->capability & WLAN_CAPABILITY_IBSS) ? 0x3 : 0x0;

	if (stats->freq == IEEE80211_52GHZ_BAND) {
		/* for A band (No DS info) */
		network->channel = stats->received_channel;
	} else
		network->flags |= NETWORK_HAS_CCK;

	network->wpa_ie_len = 0;
	network->rsn_ie_len = 0;

	if (ieee80211_parse_info_param
	    (frame->info_element, stats->len - sizeof(*frame), network))
		return 1;

	network->mode = 0;
	if (stats->freq == IEEE80211_52GHZ_BAND)
		network->mode = IEEE_A;
	else {
		if (network->flags & NETWORK_HAS_OFDM)
			network->mode |= IEEE_G;
		if (network->flags & NETWORK_HAS_CCK)
			network->mode |= IEEE_B;
	}

	if (ieee80211_is_empty_essid(network->ssid, network->ssid_len))
		network->flags |= NETWORK_EMPTY_ESSID;

	memcpy(&network->stats, stats, sizeof(network->stats));

	if (ieee->handle_assoc_response != NULL)
		ieee->handle_assoc_response(dev, frame, network);

	return 0;
}