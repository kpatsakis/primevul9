void mwifiex_process_bypass_tx(struct mwifiex_adapter *adapter)
{
	struct mwifiex_tx_param tx_param;
	struct sk_buff *skb;
	struct mwifiex_txinfo *tx_info;
	struct mwifiex_private *priv;
	int i;

	if (adapter->data_sent || adapter->tx_lock_flag)
		return;

	for (i = 0; i < adapter->priv_num; ++i) {
		priv = adapter->priv[i];

		if (!priv)
			continue;

		if (adapter->if_ops.is_port_ready &&
		    !adapter->if_ops.is_port_ready(priv))
			continue;

		if (skb_queue_empty(&priv->bypass_txq))
			continue;

		skb = skb_dequeue(&priv->bypass_txq);
		tx_info = MWIFIEX_SKB_TXCB(skb);

		/* no aggregation for bypass packets */
		tx_param.next_pkt_len = 0;

		if (mwifiex_process_tx(priv, skb, &tx_param) == -EBUSY) {
			skb_queue_head(&priv->bypass_txq, skb);
			tx_info->flags |= MWIFIEX_BUF_FLAG_REQUEUED_PKT;
		} else {
			atomic_dec(&adapter->bypass_tx_pending);
		}
	}
}