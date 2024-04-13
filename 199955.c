static int ieee80211_network_init(struct ieee80211_device *ieee, struct ieee80211_probe_response
					 *beacon,
					 struct ieee80211_network *network,
					 struct ieee80211_rx_stats *stats)
{
	network->qos_data.active = 0;
	network->qos_data.supported = 0;
	network->qos_data.param_count = 0;
	network->qos_data.old_param_count = 0;

	/* Pull out fixed field data */
	memcpy(network->bssid, beacon->header.addr3, ETH_ALEN);
	network->capability = le16_to_cpu(beacon->capability);
	network->last_scanned = jiffies;
	network->time_stamp[0] = le32_to_cpu(beacon->time_stamp[0]);
	network->time_stamp[1] = le32_to_cpu(beacon->time_stamp[1]);
	network->beacon_interval = le16_to_cpu(beacon->beacon_interval);
	/* Where to pull this? beacon->listen_interval; */
	network->listen_interval = 0x0A;
	network->rates_len = network->rates_ex_len = 0;
	network->last_associate = 0;
	network->ssid_len = 0;
	network->flags = 0;
	network->atim_window = 0;
	network->erp_value = (network->capability & WLAN_CAPABILITY_IBSS) ?
	    0x3 : 0x0;

	if (stats->freq == IEEE80211_52GHZ_BAND) {
		/* for A band (No DS info) */
		network->channel = stats->received_channel;
	} else
		network->flags |= NETWORK_HAS_CCK;

	network->wpa_ie_len = 0;
	network->rsn_ie_len = 0;

	if (ieee80211_parse_info_param
	    (beacon->info_element, stats->len - sizeof(*beacon), network))
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

	if (network->mode == 0) {
		IEEE80211_DEBUG_SCAN("Filtered out '%s (" MAC_FMT ")' "
				     "network.\n",
				     escape_essid(network->ssid,
						  network->ssid_len),
				     MAC_ARG(network->bssid));
		return 1;
	}

	if (ieee80211_is_empty_essid(network->ssid, network->ssid_len))
		network->flags |= NETWORK_EMPTY_ESSID;

	memcpy(&network->stats, stats, sizeof(network->stats));

	return 0;
}