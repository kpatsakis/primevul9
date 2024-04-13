static int mwifiex_pcie_host_to_card(struct mwifiex_adapter *adapter, u8 type,
				     struct sk_buff *skb,
				     struct mwifiex_tx_param *tx_param)
{
	if (!skb) {
		mwifiex_dbg(adapter, ERROR,
			    "Passed NULL skb to %s\n", __func__);
		return -1;
	}

	if (type == MWIFIEX_TYPE_DATA)
		return mwifiex_pcie_send_data(adapter, skb, tx_param);
	else if (type == MWIFIEX_TYPE_CMD)
		return mwifiex_pcie_send_cmd(adapter, skb);

	return 0;
}