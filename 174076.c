mwifiex_update_chan_statistics(struct mwifiex_private *priv,
			       struct mwifiex_ietypes_chanstats *tlv_stat)
{
	struct mwifiex_adapter *adapter = priv->adapter;
	u8 i, num_chan;
	struct mwifiex_fw_chan_stats *fw_chan_stats;
	struct mwifiex_chan_stats chan_stats;

	fw_chan_stats = (void *)((u8 *)tlv_stat +
			      sizeof(struct mwifiex_ie_types_header));
	num_chan = le16_to_cpu(tlv_stat->header.len) /
					      sizeof(struct mwifiex_chan_stats);

	for (i = 0 ; i < num_chan; i++) {
		if (adapter->survey_idx >= adapter->num_in_chan_stats) {
			mwifiex_dbg(adapter, WARN,
				    "FW reported too many channel results (max %d)\n",
				    adapter->num_in_chan_stats);
			return;
		}
		chan_stats.chan_num = fw_chan_stats->chan_num;
		chan_stats.bandcfg = fw_chan_stats->bandcfg;
		chan_stats.flags = fw_chan_stats->flags;
		chan_stats.noise = fw_chan_stats->noise;
		chan_stats.total_bss = le16_to_cpu(fw_chan_stats->total_bss);
		chan_stats.cca_scan_dur =
				       le16_to_cpu(fw_chan_stats->cca_scan_dur);
		chan_stats.cca_busy_dur =
				       le16_to_cpu(fw_chan_stats->cca_busy_dur);
		mwifiex_dbg(adapter, INFO,
			    "chan=%d, noise=%d, total_network=%d scan_duration=%d, busy_duration=%d\n",
			    chan_stats.chan_num,
			    chan_stats.noise,
			    chan_stats.total_bss,
			    chan_stats.cca_scan_dur,
			    chan_stats.cca_busy_dur);
		memcpy(&adapter->chan_stats[adapter->survey_idx++], &chan_stats,
		       sizeof(struct mwifiex_chan_stats));
		fw_chan_stats++;
	}
}