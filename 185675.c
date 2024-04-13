mwifiex_wmm_process_tx(struct mwifiex_adapter *adapter)
{
	do {
		if (mwifiex_dequeue_tx_packet(adapter))
			break;
		if (adapter->iface_type != MWIFIEX_SDIO) {
			if (adapter->data_sent ||
			    adapter->tx_lock_flag)
				break;
		} else {
			if (atomic_read(&adapter->tx_queued) >=
			    MWIFIEX_MAX_PKTS_TXQ)
				break;
		}
	} while (!mwifiex_wmm_lists_empty(adapter));
}