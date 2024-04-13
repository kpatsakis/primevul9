int psr_get(uint16_t *pskey, uint8_t *value, uint16_t *size)
{
	struct psr_data *item = head;

	if (!head)
		return -ENOENT;

	*pskey = item->pskey;

	if (item->value) {
		if (value && item->size > 0)
			memcpy(value, item->value, item->size);
		free(item->value);
		*size = item->size;
	} else
		*size = 0;

	if (head == tail)
		tail = NULL;

	head = head->next;
	free(item);

	return 0;
}