static void mwifiex_check_next_scan_command(struct mwifiex_private *priv)
{
	struct mwifiex_adapter *adapter = priv->adapter;
	struct cmd_ctrl_node *cmd_node;

	spin_lock_bh(&adapter->scan_pending_q_lock);
	if (list_empty(&adapter->scan_pending_q)) {
		spin_unlock_bh(&adapter->scan_pending_q_lock);

		spin_lock_bh(&adapter->mwifiex_cmd_lock);
		adapter->scan_processing = false;
		spin_unlock_bh(&adapter->mwifiex_cmd_lock);

		mwifiex_active_scan_req_for_passive_chan(priv);

		if (!adapter->ext_scan)
			mwifiex_complete_scan(priv);

		if (priv->scan_request) {
			struct cfg80211_scan_info info = {
				.aborted = false,
			};

			mwifiex_dbg(adapter, INFO,
				    "info: notifying scan done\n");
			cfg80211_scan_done(priv->scan_request, &info);
			priv->scan_request = NULL;
			priv->scan_aborting = false;
		} else {
			priv->scan_aborting = false;
			mwifiex_dbg(adapter, INFO,
				    "info: scan already aborted\n");
		}
	} else if ((priv->scan_aborting && !priv->scan_request) ||
		   priv->scan_block) {
		spin_unlock_bh(&adapter->scan_pending_q_lock);

		mwifiex_cancel_pending_scan_cmd(adapter);

		spin_lock_bh(&adapter->mwifiex_cmd_lock);
		adapter->scan_processing = false;
		spin_unlock_bh(&adapter->mwifiex_cmd_lock);

		if (!adapter->active_scan_triggered) {
			if (priv->scan_request) {
				struct cfg80211_scan_info info = {
					.aborted = true,
				};

				mwifiex_dbg(adapter, INFO,
					    "info: aborting scan\n");
				cfg80211_scan_done(priv->scan_request, &info);
				priv->scan_request = NULL;
				priv->scan_aborting = false;
			} else {
				priv->scan_aborting = false;
				mwifiex_dbg(adapter, INFO,
					    "info: scan already aborted\n");
			}
		}
	} else {
		/* Get scan command from scan_pending_q and put to
		 * cmd_pending_q
		 */
		cmd_node = list_first_entry(&adapter->scan_pending_q,
					    struct cmd_ctrl_node, list);
		list_del(&cmd_node->list);
		spin_unlock_bh(&adapter->scan_pending_q_lock);
		mwifiex_insert_cmd_to_pending_q(adapter, cmd_node);
	}

	return;
}