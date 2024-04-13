void *assoc_array_find(const struct assoc_array *array,
		       const struct assoc_array_ops *ops,
		       const void *index_key)
{
	struct assoc_array_walk_result result;
	const struct assoc_array_node *node;
	const struct assoc_array_ptr *ptr;
	const void *leaf;
	int slot;

	if (assoc_array_walk(array, ops, index_key, &result) !=
	    assoc_array_walk_found_terminal_node)
		return NULL;

	node = result.terminal_node.node;
	smp_read_barrier_depends();

	/* If the target key is available to us, it's has to be pointed to by
	 * the terminal node.
	 */
	for (slot = 0; slot < ASSOC_ARRAY_FAN_OUT; slot++) {
		ptr = ACCESS_ONCE(node->slots[slot]);
		if (ptr && assoc_array_ptr_is_leaf(ptr)) {
			/* We need a barrier between the read of the pointer
			 * and dereferencing the pointer - but only if we are
			 * actually going to dereference it.
			 */
			leaf = assoc_array_ptr_to_leaf(ptr);
			smp_read_barrier_depends();
			if (ops->compare_object(leaf, index_key))
				return (void *)leaf;
		}
	}

	return NULL;
}