			   __releases(&priv->wmm.ra_list_spinlock)
{
	struct sk_buff *skb, *skb_next;
	struct mwifiex_tx_param tx_param;
	struct mwifiex_adapter *adapter = priv->adapter;
	struct mwifiex_txinfo *tx_info;

	if (skb_queue_empty(&ptr->skb_head)) {
		spin_unlock_bh(&priv->wmm.ra_list_spinlock);
		mwifiex_dbg(adapter, DATA, "data: nothing to send\n");
		return;
	}

	skb = skb_dequeue(&ptr->skb_head);

	tx_info = MWIFIEX_SKB_TXCB(skb);
	mwifiex_dbg(adapter, DATA,
		    "data: dequeuing the packet %p %p\n", ptr, skb);

	ptr->total_pkt_count--;

	if (!skb_queue_empty(&ptr->skb_head))
		skb_next = skb_peek(&ptr->skb_head);
	else
		skb_next = NULL;

	spin_unlock_bh(&priv->wmm.ra_list_spinlock);

	tx_param.next_pkt_len = ((skb_next) ? skb_next->len +
				sizeof(struct txpd) : 0);

	if (mwifiex_process_tx(priv, skb, &tx_param) == -EBUSY) {
		/* Queue the packet back at the head */
		spin_lock_bh(&priv->wmm.ra_list_spinlock);

		if (!mwifiex_is_ralist_valid(priv, ptr, ptr_index)) {
			spin_unlock_bh(&priv->wmm.ra_list_spinlock);
			mwifiex_write_data_complete(adapter, skb, 0, -1);
			return;
		}

		skb_queue_tail(&ptr->skb_head, skb);

		ptr->total_pkt_count++;
		ptr->ba_pkt_count++;
		tx_info->flags |= MWIFIEX_BUF_FLAG_REQUEUED_PKT;
		spin_unlock_bh(&priv->wmm.ra_list_spinlock);
	} else {
		mwifiex_rotate_priolists(priv, ptr, ptr_index);
		atomic_dec(&priv->wmm.tx_pkts_queued);
	}
}