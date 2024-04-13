int psr_put(uint16_t pskey, uint8_t *value, uint16_t size)
{
	struct psr_data *item;

	item = malloc(sizeof(*item));
	if (!item)
		return -ENOMEM;

	item->pskey = pskey;

	if (size > 0) {
		item->value = malloc(size);
		if (!item->value) {
			free(item);
			return -ENOMEM;
		}

		memcpy(item->value, value, size);
		item->size = size;
	} else {
		item->value = NULL;
		item->size = 0;
	}

	item->next = NULL;

	if (!head)
		head = item;
	else
		tail->next = item;

	tail = item;

	return 0;
}