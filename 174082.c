static void mwifiex_complete_scan(struct mwifiex_private *priv)
{
	struct mwifiex_adapter *adapter = priv->adapter;

	adapter->survey_idx = 0;
	if (adapter->curr_cmd->wait_q_enabled) {
		adapter->cmd_wait_q.status = 0;
		if (!priv->scan_request) {
			mwifiex_dbg(adapter, INFO,
				    "complete internal scan\n");
			mwifiex_complete_cmd(adapter, adapter->curr_cmd);
		}
	}
}