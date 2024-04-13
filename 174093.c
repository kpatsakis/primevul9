int mwifiex_ret_802_11_scan_ext(struct mwifiex_private *priv,
				struct host_cmd_ds_command *resp)
{
	struct mwifiex_adapter *adapter = priv->adapter;
	struct host_cmd_ds_802_11_scan_ext *ext_scan_resp;
	struct mwifiex_ie_types_header *tlv;
	struct mwifiex_ietypes_chanstats *tlv_stat;
	u16 buf_left, type, len;

	struct host_cmd_ds_command *cmd_ptr;
	struct cmd_ctrl_node *cmd_node;
	bool complete_scan = false;

	mwifiex_dbg(adapter, INFO, "info: EXT scan returns successfully\n");

	ext_scan_resp = &resp->params.ext_scan;

	tlv = (void *)ext_scan_resp->tlv_buffer;
	buf_left = le16_to_cpu(resp->size) - (sizeof(*ext_scan_resp) + S_DS_GEN
					      - 1);

	while (buf_left >= sizeof(struct mwifiex_ie_types_header)) {
		type = le16_to_cpu(tlv->type);
		len = le16_to_cpu(tlv->len);

		if (buf_left < (sizeof(struct mwifiex_ie_types_header) + len)) {
			mwifiex_dbg(adapter, ERROR,
				    "error processing scan response TLVs");
			break;
		}

		switch (type) {
		case TLV_TYPE_CHANNEL_STATS:
			tlv_stat = (void *)tlv;
			mwifiex_update_chan_statistics(priv, tlv_stat);
			break;
		default:
			break;
		}

		buf_left -= len + sizeof(struct mwifiex_ie_types_header);
		tlv = (void *)((u8 *)tlv + len +
			       sizeof(struct mwifiex_ie_types_header));
	}

	spin_lock_bh(&adapter->cmd_pending_q_lock);
	spin_lock_bh(&adapter->scan_pending_q_lock);
	if (list_empty(&adapter->scan_pending_q)) {
		complete_scan = true;
		list_for_each_entry(cmd_node, &adapter->cmd_pending_q, list) {
			cmd_ptr = (void *)cmd_node->cmd_skb->data;
			if (le16_to_cpu(cmd_ptr->command) ==
			    HostCmd_CMD_802_11_SCAN_EXT) {
				mwifiex_dbg(adapter, INFO,
					    "Scan pending in command pending list");
				complete_scan = false;
				break;
			}
		}
	}
	spin_unlock_bh(&adapter->scan_pending_q_lock);
	spin_unlock_bh(&adapter->cmd_pending_q_lock);

	if (complete_scan)
		mwifiex_complete_scan(priv);

	return 0;
}