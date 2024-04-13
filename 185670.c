mwifiex_wmm_add_buf_txqueue(struct mwifiex_private *priv,
			    struct sk_buff *skb)
{
	struct mwifiex_adapter *adapter = priv->adapter;
	u32 tid;
	struct mwifiex_ra_list_tbl *ra_list;
	u8 ra[ETH_ALEN], tid_down;
	struct list_head list_head;
	int tdls_status = TDLS_NOT_SETUP;
	struct ethhdr *eth_hdr = (struct ethhdr *)skb->data;
	struct mwifiex_txinfo *tx_info = MWIFIEX_SKB_TXCB(skb);

	memcpy(ra, eth_hdr->h_dest, ETH_ALEN);

	if (GET_BSS_ROLE(priv) == MWIFIEX_BSS_ROLE_STA &&
	    ISSUPP_TDLS_ENABLED(adapter->fw_cap_info)) {
		if (ntohs(eth_hdr->h_proto) == ETH_P_TDLS)
			mwifiex_dbg(adapter, DATA,
				    "TDLS setup packet for %pM.\t"
				    "Don't block\n", ra);
		else if (memcmp(priv->cfg_bssid, ra, ETH_ALEN))
			tdls_status = mwifiex_get_tdls_link_status(priv, ra);
	}

	if (!priv->media_connected && !mwifiex_is_skb_mgmt_frame(skb)) {
		mwifiex_dbg(adapter, DATA, "data: drop packet in disconnect\n");
		mwifiex_write_data_complete(adapter, skb, 0, -1);
		return;
	}

	tid = skb->priority;

	spin_lock_bh(&priv->wmm.ra_list_spinlock);

	tid_down = mwifiex_wmm_downgrade_tid(priv, tid);

	/* In case of infra as we have already created the list during
	   association we just don't have to call get_queue_raptr, we will
	   have only 1 raptr for a tid in case of infra */
	if (!mwifiex_queuing_ra_based(priv) &&
	    !mwifiex_is_skb_mgmt_frame(skb)) {
		switch (tdls_status) {
		case TDLS_SETUP_COMPLETE:
		case TDLS_CHAN_SWITCHING:
		case TDLS_IN_BASE_CHAN:
		case TDLS_IN_OFF_CHAN:
			ra_list = mwifiex_wmm_get_queue_raptr(priv, tid_down,
							      ra);
			tx_info->flags |= MWIFIEX_BUF_FLAG_TDLS_PKT;
			break;
		case TDLS_SETUP_INPROGRESS:
			skb_queue_tail(&priv->tdls_txq, skb);
			spin_unlock_bh(&priv->wmm.ra_list_spinlock);
			return;
		default:
			list_head = priv->wmm.tid_tbl_ptr[tid_down].ra_list;
			ra_list = list_first_entry_or_null(&list_head,
					struct mwifiex_ra_list_tbl, list);
			break;
		}
	} else {
		memcpy(ra, skb->data, ETH_ALEN);
		if (ra[0] & 0x01 || mwifiex_is_skb_mgmt_frame(skb))
			eth_broadcast_addr(ra);
		ra_list = mwifiex_wmm_get_queue_raptr(priv, tid_down, ra);
	}

	if (!ra_list) {
		spin_unlock_bh(&priv->wmm.ra_list_spinlock);
		mwifiex_write_data_complete(adapter, skb, 0, -1);
		return;
	}

	skb_queue_tail(&ra_list->skb_head, skb);

	ra_list->ba_pkt_count++;
	ra_list->total_pkt_count++;

	if (atomic_read(&priv->wmm.highest_queued_prio) <
						priv->tos_to_tid_inv[tid_down])
		atomic_set(&priv->wmm.highest_queued_prio,
			   priv->tos_to_tid_inv[tid_down]);

	if (ra_list->tx_paused)
		priv->wmm.pkts_paused[tid_down]++;
	else
		atomic_inc(&priv->wmm.tx_pkts_queued);

	spin_unlock_bh(&priv->wmm.ra_list_spinlock);
}