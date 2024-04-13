static void *aarp_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	struct aarp_entry *entry = v;
	struct aarp_iter_state *iter = seq->private;

	++*pos;

	/* first line after header */
	if (v == SEQ_START_TOKEN)
		entry = iter_next(iter, NULL);

	/* next entry in current bucket */
	else if (entry->next)
		entry = entry->next;

	/* next bucket or table */
	else {
		++iter->bucket;
		entry = iter_next(iter, NULL);
	}
	return entry;
}