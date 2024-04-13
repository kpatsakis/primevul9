				__releases(&priv->wmm.ra_list_spinlock)
{
	struct mwifiex_tx_param tx_param;
	struct mwifiex_adapter *adapter = priv->adapter;
	int ret = -1;
	struct sk_buff *skb, *skb_next;
	struct mwifiex_txinfo *tx_info;

	if (skb_queue_empty(&ptr->skb_head)) {
		spin_unlock_bh(&priv->wmm.ra_list_spinlock);
		return;
	}

	skb = skb_dequeue(&ptr->skb_head);

	if (adapter->data_sent || adapter->tx_lock_flag) {
		ptr->total_pkt_count--;
		spin_unlock_bh(&priv->wmm.ra_list_spinlock);
		skb_queue_tail(&adapter->tx_data_q, skb);
		atomic_dec(&priv->wmm.tx_pkts_queued);
		atomic_inc(&adapter->tx_queued);
		return;
	}

	if (!skb_queue_empty(&ptr->skb_head))
		skb_next = skb_peek(&ptr->skb_head);
	else
		skb_next = NULL;

	tx_info = MWIFIEX_SKB_TXCB(skb);

	spin_unlock_bh(&priv->wmm.ra_list_spinlock);

	tx_param.next_pkt_len =
		((skb_next) ? skb_next->len +
		 sizeof(struct txpd) : 0);
	if (adapter->iface_type == MWIFIEX_USB) {
		ret = adapter->if_ops.host_to_card(adapter, priv->usb_port,
						   skb, &tx_param);
	} else {
		ret = adapter->if_ops.host_to_card(adapter, MWIFIEX_TYPE_DATA,
						   skb, &tx_param);
	}

	switch (ret) {
	case -EBUSY:
		mwifiex_dbg(adapter, ERROR, "data: -EBUSY is returned\n");
		spin_lock_bh(&priv->wmm.ra_list_spinlock);

		if (!mwifiex_is_ralist_valid(priv, ptr, ptr_index)) {
			spin_unlock_bh(&priv->wmm.ra_list_spinlock);
			mwifiex_write_data_complete(adapter, skb, 0, -1);
			return;
		}

		skb_queue_tail(&ptr->skb_head, skb);

		tx_info->flags |= MWIFIEX_BUF_FLAG_REQUEUED_PKT;
		spin_unlock_bh(&priv->wmm.ra_list_spinlock);
		break;
	case -1:
		mwifiex_dbg(adapter, ERROR, "host_to_card failed: %#x\n", ret);
		adapter->dbg.num_tx_host_to_card_failure++;
		mwifiex_write_data_complete(adapter, skb, 0, ret);
		break;
	case -EINPROGRESS:
		break;
	case 0:
		mwifiex_write_data_complete(adapter, skb, 0, ret);
	default:
		break;
	}
	if (ret != -EBUSY) {
		mwifiex_rotate_priolists(priv, ptr, ptr_index);
		atomic_dec(&priv->wmm.tx_pkts_queued);
		spin_lock_bh(&priv->wmm.ra_list_spinlock);
		ptr->total_pkt_count--;
		spin_unlock_bh(&priv->wmm.ra_list_spinlock);
	}
}