nfqnl_flush(struct nfqnl_instance *queue, nfqnl_cmpfn cmpfn, unsigned long data)
{
	struct nf_queue_entry *entry, *next;

	spin_lock_bh(&queue->lock);
	list_for_each_entry_safe(entry, next, &queue->queue_list, list) {
		if (!cmpfn || cmpfn(entry, data)) {
			list_del(&entry->list);
			queue->queue_total--;
			nf_reinject(entry, NF_DROP);
		}
	}
	spin_unlock_bh(&queue->lock);
}