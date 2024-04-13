void mwifiex_cancel_scan(struct mwifiex_adapter *adapter)
{
	struct mwifiex_private *priv;
	int i;

	mwifiex_cancel_pending_scan_cmd(adapter);

	if (adapter->scan_processing) {
		spin_lock_bh(&adapter->mwifiex_cmd_lock);
		adapter->scan_processing = false;
		spin_unlock_bh(&adapter->mwifiex_cmd_lock);
		for (i = 0; i < adapter->priv_num; i++) {
			priv = adapter->priv[i];
			if (!priv)
				continue;
			if (priv->scan_request) {
				struct cfg80211_scan_info info = {
					.aborted = true,
				};

				mwifiex_dbg(adapter, INFO,
					    "info: aborting scan\n");
				cfg80211_scan_done(priv->scan_request, &info);
				priv->scan_request = NULL;
				priv->scan_aborting = false;
			}
		}
	}
}