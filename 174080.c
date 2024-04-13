mwifiex_queue_scan_cmd(struct mwifiex_private *priv,
		       struct cmd_ctrl_node *cmd_node)
{
	struct mwifiex_adapter *adapter = priv->adapter;

	cmd_node->wait_q_enabled = true;
	cmd_node->condition = &adapter->scan_wait_q_woken;
	spin_lock_bh(&adapter->scan_pending_q_lock);
	list_add_tail(&cmd_node->list, &adapter->scan_pending_q);
	spin_unlock_bh(&adapter->scan_pending_q_lock);
}