static void sdma_flush(struct sdma_engine *sde)
{
	struct sdma_txreq *txp, *txp_next;
	LIST_HEAD(flushlist);
	unsigned long flags;
	uint seq;

	/* flush from head to tail */
	sdma_flush_descq(sde);
	spin_lock_irqsave(&sde->flushlist_lock, flags);
	/* copy flush list */
	list_splice_init(&sde->flushlist, &flushlist);
	spin_unlock_irqrestore(&sde->flushlist_lock, flags);
	/* flush from flush list */
	list_for_each_entry_safe(txp, txp_next, &flushlist, list)
		complete_tx(sde, txp, SDMA_TXREQ_S_ABORTED);
	/* wakeup QPs orphaned on the dmawait list */
	do {
		struct iowait *w, *nw;

		seq = read_seqbegin(&sde->waitlock);
		if (!list_empty(&sde->dmawait)) {
			write_seqlock(&sde->waitlock);
			list_for_each_entry_safe(w, nw, &sde->dmawait, list) {
				if (w->wakeup) {
					w->wakeup(w, SDMA_AVAIL_REASON);
					list_del_init(&w->list);
				}
			}
			write_sequnlock(&sde->waitlock);
		}
	} while (read_seqretry(&sde->waitlock, seq));
}