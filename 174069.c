static int mwifiex_scan_specific_ssid(struct mwifiex_private *priv,
				      struct cfg80211_ssid *req_ssid)
{
	struct mwifiex_adapter *adapter = priv->adapter;
	int ret;
	struct mwifiex_user_scan_cfg *scan_cfg;

	if (adapter->scan_processing) {
		mwifiex_dbg(adapter, WARN,
			    "cmd: Scan already in process...\n");
		return -EBUSY;
	}

	if (priv->scan_block) {
		mwifiex_dbg(adapter, WARN,
			    "cmd: Scan is blocked during association...\n");
		return -EBUSY;
	}

	scan_cfg = kzalloc(sizeof(struct mwifiex_user_scan_cfg), GFP_KERNEL);
	if (!scan_cfg)
		return -ENOMEM;

	scan_cfg->ssid_list = req_ssid;
	scan_cfg->num_ssids = 1;

	ret = mwifiex_scan_networks(priv, scan_cfg);

	kfree(scan_cfg);
	return ret;
}