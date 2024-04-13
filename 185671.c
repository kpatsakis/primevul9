mwifiex_wmm_queue_priorities_tid(struct mwifiex_private *priv)
{
	struct mwifiex_wmm_desc *wmm = &priv->wmm;
	u8 *queue_priority = wmm->queue_priority;
	int i;

	for (i = 0; i < 4; ++i) {
		tos_to_tid[7 - (i * 2)] = ac_to_tid[queue_priority[i]][1];
		tos_to_tid[6 - (i * 2)] = ac_to_tid[queue_priority[i]][0];
	}

	for (i = 0; i < MAX_NUM_TID; ++i)
		priv->tos_to_tid_inv[tos_to_tid[i]] = (u8)i;

	atomic_set(&wmm->highest_queued_prio, HIGH_PRIO_TID);
}