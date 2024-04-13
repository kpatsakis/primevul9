static void ieee80211_process_probe_response(struct ieee80211_device
						    *ieee, struct
						    ieee80211_probe_response
						    *beacon, struct ieee80211_rx_stats
						    *stats)
{
	struct net_device *dev = ieee->dev;
	struct ieee80211_network network = {
		.ibss_dfs = NULL,
	};
	struct ieee80211_network *target;
	struct ieee80211_network *oldest = NULL;
#ifdef CONFIG_IEEE80211_DEBUG
	struct ieee80211_info_element *info_element = beacon->info_element;
#endif
	unsigned long flags;

	IEEE80211_DEBUG_SCAN("'%s' (" MAC_FMT
			     "): %c%c%c%c %c%c%c%c-%c%c%c%c %c%c%c%c\n",
			     escape_essid(info_element->data,
					  info_element->len),
			     MAC_ARG(beacon->header.addr3),
			     (beacon->capability & (1 << 0xf)) ? '1' : '0',
			     (beacon->capability & (1 << 0xe)) ? '1' : '0',
			     (beacon->capability & (1 << 0xd)) ? '1' : '0',
			     (beacon->capability & (1 << 0xc)) ? '1' : '0',
			     (beacon->capability & (1 << 0xb)) ? '1' : '0',
			     (beacon->capability & (1 << 0xa)) ? '1' : '0',
			     (beacon->capability & (1 << 0x9)) ? '1' : '0',
			     (beacon->capability & (1 << 0x8)) ? '1' : '0',
			     (beacon->capability & (1 << 0x7)) ? '1' : '0',
			     (beacon->capability & (1 << 0x6)) ? '1' : '0',
			     (beacon->capability & (1 << 0x5)) ? '1' : '0',
			     (beacon->capability & (1 << 0x4)) ? '1' : '0',
			     (beacon->capability & (1 << 0x3)) ? '1' : '0',
			     (beacon->capability & (1 << 0x2)) ? '1' : '0',
			     (beacon->capability & (1 << 0x1)) ? '1' : '0',
			     (beacon->capability & (1 << 0x0)) ? '1' : '0');

	if (ieee80211_network_init(ieee, beacon, &network, stats)) {
		IEEE80211_DEBUG_SCAN("Dropped '%s' (" MAC_FMT ") via %s.\n",
				     escape_essid(info_element->data,
						  info_element->len),
				     MAC_ARG(beacon->header.addr3),
				     is_beacon(beacon->header.frame_ctl) ?
				     "BEACON" : "PROBE RESPONSE");
		return;
	}

	/* The network parsed correctly -- so now we scan our known networks
	 * to see if we can find it in our list.
	 *
	 * NOTE:  This search is definitely not optimized.  Once its doing
	 *        the "right thing" we'll optimize it for efficiency if
	 *        necessary */

	/* Search for this entry in the list and update it if it is
	 * already there. */

	spin_lock_irqsave(&ieee->lock, flags);

	list_for_each_entry(target, &ieee->network_list, list) {
		if (is_same_network(target, &network))
			break;

		if ((oldest == NULL) ||
		    (target->last_scanned < oldest->last_scanned))
			oldest = target;
	}

	/* If we didn't find a match, then get a new network slot to initialize
	 * with this beacon's information */
	if (&target->list == &ieee->network_list) {
		if (list_empty(&ieee->network_free_list)) {
			/* If there are no more slots, expire the oldest */
			list_del(&oldest->list);
			target = oldest;
			IEEE80211_DEBUG_SCAN("Expired '%s' (" MAC_FMT ") from "
					     "network list.\n",
					     escape_essid(target->ssid,
							  target->ssid_len),
					     MAC_ARG(target->bssid));
			ieee80211_network_reset(target);
		} else {
			/* Otherwise just pull from the free list */
			target = list_entry(ieee->network_free_list.next,
					    struct ieee80211_network, list);
			list_del(ieee->network_free_list.next);
		}

#ifdef CONFIG_IEEE80211_DEBUG
		IEEE80211_DEBUG_SCAN("Adding '%s' (" MAC_FMT ") via %s.\n",
				     escape_essid(network.ssid,
						  network.ssid_len),
				     MAC_ARG(network.bssid),
				     is_beacon(beacon->header.frame_ctl) ?
				     "BEACON" : "PROBE RESPONSE");
#endif
		memcpy(target, &network, sizeof(*target));
		network.ibss_dfs = NULL;
		list_add_tail(&target->list, &ieee->network_list);
	} else {
		IEEE80211_DEBUG_SCAN("Updating '%s' (" MAC_FMT ") via %s.\n",
				     escape_essid(target->ssid,
						  target->ssid_len),
				     MAC_ARG(target->bssid),
				     is_beacon(beacon->header.frame_ctl) ?
				     "BEACON" : "PROBE RESPONSE");
		update_network(target, &network);
		network.ibss_dfs = NULL;
	}

	spin_unlock_irqrestore(&ieee->lock, flags);

	if (is_beacon(beacon->header.frame_ctl)) {
		if (ieee->handle_beacon != NULL)
			ieee->handle_beacon(dev, beacon, target);
	} else {
		if (ieee->handle_probe_response != NULL)
			ieee->handle_probe_response(dev, beacon, target);
	}
}