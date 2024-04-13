void mwifiex_rotate_priolists(struct mwifiex_private *priv,
				 struct mwifiex_ra_list_tbl *ra,
				 int tid)
{
	struct mwifiex_adapter *adapter = priv->adapter;
	struct mwifiex_bss_prio_tbl *tbl = adapter->bss_prio_tbl;
	struct mwifiex_tid_tbl *tid_ptr = &priv->wmm.tid_tbl_ptr[tid];

	spin_lock_bh(&tbl[priv->bss_priority].bss_prio_lock);
	/*
	 * dirty trick: we remove 'head' temporarily and reinsert it after
	 * curr bss node. imagine list to stay fixed while head is moved
	 */
	list_move(&tbl[priv->bss_priority].bss_prio_head,
		  &tbl[priv->bss_priority].bss_prio_cur->list);
	spin_unlock_bh(&tbl[priv->bss_priority].bss_prio_lock);

	spin_lock_bh(&priv->wmm.ra_list_spinlock);
	if (mwifiex_is_ralist_valid(priv, ra, tid)) {
		priv->wmm.packets_out[tid]++;
		/* same as above */
		list_move(&tid_ptr->ra_list, &ra->list);
	}
	spin_unlock_bh(&priv->wmm.ra_list_spinlock);
}