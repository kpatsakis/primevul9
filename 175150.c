nf_queue_entry_dup(struct nf_queue_entry *e)
{
	struct nf_queue_entry *entry = kmemdup(e, e->size, GFP_ATOMIC);
	if (entry) {
		if (nf_queue_entry_get_refs(entry))
			return entry;
		kfree(entry);
	}
	return NULL;
}