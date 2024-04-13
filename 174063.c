mwifiex_bgscan_create_channel_list(struct mwifiex_private *priv,
				   const struct mwifiex_bg_scan_cfg
						*bgscan_cfg_in,
				   struct mwifiex_chan_scan_param_set
						*scan_chan_list)
{
	enum nl80211_band band;
	struct ieee80211_supported_band *sband;
	struct ieee80211_channel *ch;
	struct mwifiex_adapter *adapter = priv->adapter;
	int chan_idx = 0, i;

	for (band = 0; (band < NUM_NL80211_BANDS); band++) {
		if (!priv->wdev.wiphy->bands[band])
			continue;

		sband = priv->wdev.wiphy->bands[band];

		for (i = 0; (i < sband->n_channels) ; i++) {
			ch = &sband->channels[i];
			if (ch->flags & IEEE80211_CHAN_DISABLED)
				continue;
			scan_chan_list[chan_idx].radio_type = band;

			if (bgscan_cfg_in->chan_list[0].scan_time)
				scan_chan_list[chan_idx].max_scan_time =
					cpu_to_le16((u16)bgscan_cfg_in->
					chan_list[0].scan_time);
			else if (ch->flags & IEEE80211_CHAN_NO_IR)
				scan_chan_list[chan_idx].max_scan_time =
					cpu_to_le16(adapter->passive_scan_time);
			else
				scan_chan_list[chan_idx].max_scan_time =
					cpu_to_le16(adapter->
						    specific_scan_time);

			if (ch->flags & IEEE80211_CHAN_NO_IR)
				scan_chan_list[chan_idx].chan_scan_mode_bitmap
					|= MWIFIEX_PASSIVE_SCAN;
			else
				scan_chan_list[chan_idx].chan_scan_mode_bitmap
					&= ~MWIFIEX_PASSIVE_SCAN;

			scan_chan_list[chan_idx].chan_number =
							(u32)ch->hw_value;
			chan_idx++;
		}
	}
	return chan_idx;
}