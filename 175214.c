find_dequeue_entry(struct nfqnl_instance *queue, unsigned int id)
{
	struct nf_queue_entry *entry = NULL, *i;

	spin_lock_bh(&queue->lock);

	list_for_each_entry(i, &queue->queue_list, list) {
		if (i->id == id) {
			entry = i;
			break;
		}
	}

	if (entry)
		__dequeue_entry(queue, entry);

	spin_unlock_bh(&queue->lock);

	return entry;
}