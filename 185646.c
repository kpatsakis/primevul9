mwifiex_wmm_get_highest_priolist_ptr(struct mwifiex_adapter *adapter,
				     struct mwifiex_private **priv, int *tid)
{
	struct mwifiex_private *priv_tmp;
	struct mwifiex_ra_list_tbl *ptr;
	struct mwifiex_tid_tbl *tid_ptr;
	atomic_t *hqp;
	int i, j;

	/* check the BSS with highest priority first */
	for (j = adapter->priv_num - 1; j >= 0; --j) {
		/* iterate over BSS with the equal priority */
		list_for_each_entry(adapter->bss_prio_tbl[j].bss_prio_cur,
				    &adapter->bss_prio_tbl[j].bss_prio_head,
				    list) {

try_again:
			priv_tmp = adapter->bss_prio_tbl[j].bss_prio_cur->priv;

			if (((priv_tmp->bss_mode != NL80211_IFTYPE_ADHOC) &&
			     !priv_tmp->port_open) ||
			    (atomic_read(&priv_tmp->wmm.tx_pkts_queued) == 0))
				continue;

			if (adapter->if_ops.is_port_ready &&
			    !adapter->if_ops.is_port_ready(priv_tmp))
				continue;

			/* iterate over the WMM queues of the BSS */
			hqp = &priv_tmp->wmm.highest_queued_prio;
			for (i = atomic_read(hqp); i >= LOW_PRIO_TID; --i) {

				spin_lock_bh(&priv_tmp->wmm.ra_list_spinlock);

				tid_ptr = &(priv_tmp)->wmm.
					tid_tbl_ptr[tos_to_tid[i]];

				/* iterate over receiver addresses */
				list_for_each_entry(ptr, &tid_ptr->ra_list,
						    list) {

					if (!ptr->tx_paused &&
					    !skb_queue_empty(&ptr->skb_head))
						/* holds both locks */
						goto found;
				}

				spin_unlock_bh(&priv_tmp->wmm.ra_list_spinlock);
			}

			if (atomic_read(&priv_tmp->wmm.tx_pkts_queued) != 0) {
				atomic_set(&priv_tmp->wmm.highest_queued_prio,
					   HIGH_PRIO_TID);
				/* Iterate current private once more, since
				 * there still exist packets in data queue
				 */
				goto try_again;
			} else
				atomic_set(&priv_tmp->wmm.highest_queued_prio,
					   NO_PKT_PRIO_TID);
		}
	}

	return NULL;

found:
	/* holds ra_list_spinlock */
	if (atomic_read(hqp) > i)
		atomic_set(hqp, i);
	spin_unlock_bh(&priv_tmp->wmm.ra_list_spinlock);

	*priv = priv_tmp;
	*tid = tos_to_tid[i];

	return ptr;
}