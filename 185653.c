mwifiex_wmm_lists_empty(struct mwifiex_adapter *adapter)
{
	int i;
	struct mwifiex_private *priv;

	for (i = 0; i < adapter->priv_num; ++i) {
		priv = adapter->priv[i];
		if (!priv)
			continue;
		if (!priv->port_open &&
		    (priv->bss_mode != NL80211_IFTYPE_ADHOC))
			continue;
		if (adapter->if_ops.is_port_ready &&
		    !adapter->if_ops.is_port_ready(priv))
			continue;
		if (atomic_read(&priv->wmm.tx_pkts_queued))
			return false;
	}

	return true;
}