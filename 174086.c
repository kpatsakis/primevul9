mwifiex_scan_create_channel_list(struct mwifiex_private *priv,
				 const struct mwifiex_user_scan_cfg
							*user_scan_in,
				 struct mwifiex_chan_scan_param_set
							*scan_chan_list,
				 u8 filtered_scan)
{
	enum nl80211_band band;
	struct ieee80211_supported_band *sband;
	struct ieee80211_channel *ch;
	struct mwifiex_adapter *adapter = priv->adapter;
	int chan_idx = 0, i;

	for (band = 0; (band < NUM_NL80211_BANDS) ; band++) {

		if (!priv->wdev.wiphy->bands[band])
			continue;

		sband = priv->wdev.wiphy->bands[band];

		for (i = 0; (i < sband->n_channels) ; i++) {
			ch = &sband->channels[i];
			if (ch->flags & IEEE80211_CHAN_DISABLED)
				continue;
			scan_chan_list[chan_idx].radio_type = band;

			if (user_scan_in &&
			    user_scan_in->chan_list[0].scan_time)
				scan_chan_list[chan_idx].max_scan_time =
					cpu_to_le16((u16) user_scan_in->
					chan_list[0].scan_time);
			else if ((ch->flags & IEEE80211_CHAN_NO_IR) ||
				 (ch->flags & IEEE80211_CHAN_RADAR))
				scan_chan_list[chan_idx].max_scan_time =
					cpu_to_le16(adapter->passive_scan_time);
			else
				scan_chan_list[chan_idx].max_scan_time =
					cpu_to_le16(adapter->active_scan_time);

			if (ch->flags & IEEE80211_CHAN_NO_IR)
				scan_chan_list[chan_idx].chan_scan_mode_bitmap
					|= (MWIFIEX_PASSIVE_SCAN |
					    MWIFIEX_HIDDEN_SSID_REPORT);
			else
				scan_chan_list[chan_idx].chan_scan_mode_bitmap
					&= ~MWIFIEX_PASSIVE_SCAN;
			scan_chan_list[chan_idx].chan_number =
							(u32) ch->hw_value;

			scan_chan_list[chan_idx].chan_scan_mode_bitmap
					|= MWIFIEX_DISABLE_CHAN_FILT;

			if (filtered_scan &&
			    !((ch->flags & IEEE80211_CHAN_NO_IR) ||
			      (ch->flags & IEEE80211_CHAN_RADAR)))
				scan_chan_list[chan_idx].max_scan_time =
				cpu_to_le16(adapter->specific_scan_time);

			chan_idx++;
		}

	}
	return chan_idx;
}