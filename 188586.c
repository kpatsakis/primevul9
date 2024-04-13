int assoc_array_iterate(const struct assoc_array *array,
			int (*iterator)(const void *object,
					void *iterator_data),
			void *iterator_data)
{
	struct assoc_array_ptr *root = ACCESS_ONCE(array->root);

	if (!root)
		return 0;
	return assoc_array_subtree_iterate(root, NULL, iterator, iterator_data);
}