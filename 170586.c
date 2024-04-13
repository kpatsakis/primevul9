static struct aarp_entry *iter_next(struct aarp_iter_state *iter, loff_t *pos)
{
	int ct = iter->bucket;
	struct aarp_entry **table = iter->table;
	loff_t off = 0;
	struct aarp_entry *entry;

 rescan:
	while (ct < AARP_HASH_SIZE) {
		for (entry = table[ct]; entry; entry = entry->next) {
			if (!pos || ++off == *pos) {
				iter->table = table;
				iter->bucket = ct;
				return entry;
			}
		}
		++ct;
	}

	if (table == resolved) {
		ct = 0;
		table = unresolved;
		goto rescan;
	}
	if (table == unresolved) {
		ct = 0;
		table = proxies;
		goto rescan;
	}
	return NULL;
}