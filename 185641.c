int mwifiex_bypass_txlist_empty(struct mwifiex_adapter *adapter)
{
	struct mwifiex_private *priv;
	int i;

	for (i = 0; i < adapter->priv_num; i++) {
		priv = adapter->priv[i];
		if (!priv)
			continue;
		if (adapter->if_ops.is_port_ready &&
		    !adapter->if_ops.is_port_ready(priv))
			continue;
		if (!skb_queue_empty(&priv->bypass_txq))
			return false;
	}

	return true;
}