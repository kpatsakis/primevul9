__dequeue_entry(struct nfqnl_instance *queue, struct nf_queue_entry *entry)
{
	list_del(&entry->list);
	queue->queue_total--;
}