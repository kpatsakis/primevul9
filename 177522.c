free_footnote_list(struct footnote_list *list, int free_refs)
{
	struct footnote_item *item = list->head;
	struct footnote_item *next;

	while (item) {
		next = item->next;
		if (free_refs)
			free_footnote_ref(item->ref);
		free(item);
		item = next;
	}
}