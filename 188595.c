static int assoc_array_delete_collapse_iterator(const void *leaf,
						void *iterator_data)
{
	struct assoc_array_delete_collapse_context *collapse = iterator_data;

	if (leaf == collapse->skip_leaf)
		return 0;

	BUG_ON(collapse->slot >= ASSOC_ARRAY_FAN_OUT);

	collapse->node->slots[collapse->slot++] = assoc_array_leaf_to_ptr(leaf);
	return 0;
}