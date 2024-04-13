find_footnote_ref(struct footnote_list *list, uint8_t *name, size_t length)
{
	unsigned int hash = hash_link_ref(name, length);
	struct footnote_item *item = NULL;

	item = list->head;

	while (item != NULL) {
		if (item->ref->id == hash)
			return item->ref;
		item = item->next;
	}

	return NULL;
}