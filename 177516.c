add_footnote_ref(struct footnote_list *list, struct footnote_ref *ref)
{
	struct footnote_item *item = calloc(1, sizeof(struct footnote_item));
	if (!item)
		return 0;
	item->ref = ref;

	if (list->head == NULL) {
		list->head = list->tail = item;
	} else {
		list->tail->next = item;
		list->tail = item;
	}
	list->count++;

	return 1;
}