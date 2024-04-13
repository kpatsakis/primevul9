assoc_array_walk(const struct assoc_array *array,
		 const struct assoc_array_ops *ops,
		 const void *index_key,
		 struct assoc_array_walk_result *result)
{
	struct assoc_array_shortcut *shortcut;
	struct assoc_array_node *node;
	struct assoc_array_ptr *cursor, *ptr;
	unsigned long sc_segments, dissimilarity;
	unsigned long segments;
	int level, sc_level, next_sc_level;
	int slot;

	pr_devel("-->%s()\n", __func__);

	cursor = ACCESS_ONCE(array->root);
	if (!cursor)
		return assoc_array_walk_tree_empty;

	level = 0;

	/* Use segments from the key for the new leaf to navigate through the
	 * internal tree, skipping through nodes and shortcuts that are on
	 * route to the destination.  Eventually we'll come to a slot that is
	 * either empty or contains a leaf at which point we've found a node in
	 * which the leaf we're looking for might be found or into which it
	 * should be inserted.
	 */
jumped:
	segments = ops->get_key_chunk(index_key, level);
	pr_devel("segments[%d]: %lx\n", level, segments);

	if (assoc_array_ptr_is_shortcut(cursor))
		goto follow_shortcut;

consider_node:
	node = assoc_array_ptr_to_node(cursor);
	smp_read_barrier_depends();

	slot = segments >> (level & ASSOC_ARRAY_KEY_CHUNK_MASK);
	slot &= ASSOC_ARRAY_FAN_MASK;
	ptr = ACCESS_ONCE(node->slots[slot]);

	pr_devel("consider slot %x [ix=%d type=%lu]\n",
		 slot, level, (unsigned long)ptr & 3);

	if (!assoc_array_ptr_is_meta(ptr)) {
		/* The node doesn't have a node/shortcut pointer in the slot
		 * corresponding to the index key that we have to follow.
		 */
		result->terminal_node.node = node;
		result->terminal_node.level = level;
		result->terminal_node.slot = slot;
		pr_devel("<--%s() = terminal_node\n", __func__);
		return assoc_array_walk_found_terminal_node;
	}

	if (assoc_array_ptr_is_node(ptr)) {
		/* There is a pointer to a node in the slot corresponding to
		 * this index key segment, so we need to follow it.
		 */
		cursor = ptr;
		level += ASSOC_ARRAY_LEVEL_STEP;
		if ((level & ASSOC_ARRAY_KEY_CHUNK_MASK) != 0)
			goto consider_node;
		goto jumped;
	}

	/* There is a shortcut in the slot corresponding to the index key
	 * segment.  We follow the shortcut if its partial index key matches
	 * this leaf's.  Otherwise we need to split the shortcut.
	 */
	cursor = ptr;
follow_shortcut:
	shortcut = assoc_array_ptr_to_shortcut(cursor);
	smp_read_barrier_depends();
	pr_devel("shortcut to %d\n", shortcut->skip_to_level);
	sc_level = level + ASSOC_ARRAY_LEVEL_STEP;
	BUG_ON(sc_level > shortcut->skip_to_level);

	do {
		/* Check the leaf against the shortcut's index key a word at a
		 * time, trimming the final word (the shortcut stores the index
		 * key completely from the root to the shortcut's target).
		 */
		if ((sc_level & ASSOC_ARRAY_KEY_CHUNK_MASK) == 0)
			segments = ops->get_key_chunk(index_key, sc_level);

		sc_segments = shortcut->index_key[sc_level >> ASSOC_ARRAY_KEY_CHUNK_SHIFT];
		dissimilarity = segments ^ sc_segments;

		if (round_up(sc_level, ASSOC_ARRAY_KEY_CHUNK_SIZE) > shortcut->skip_to_level) {
			/* Trim segments that are beyond the shortcut */
			int shift = shortcut->skip_to_level & ASSOC_ARRAY_KEY_CHUNK_MASK;
			dissimilarity &= ~(ULONG_MAX << shift);
			next_sc_level = shortcut->skip_to_level;
		} else {
			next_sc_level = sc_level + ASSOC_ARRAY_KEY_CHUNK_SIZE;
			next_sc_level = round_down(next_sc_level, ASSOC_ARRAY_KEY_CHUNK_SIZE);
		}

		if (dissimilarity != 0) {
			/* This shortcut points elsewhere */
			result->wrong_shortcut.shortcut = shortcut;
			result->wrong_shortcut.level = level;
			result->wrong_shortcut.sc_level = sc_level;
			result->wrong_shortcut.sc_segments = sc_segments;
			result->wrong_shortcut.dissimilarity = dissimilarity;
			return assoc_array_walk_found_wrong_shortcut;
		}

		sc_level = next_sc_level;
	} while (sc_level < shortcut->skip_to_level);

	/* The shortcut matches the leaf's index to this point. */
	cursor = ACCESS_ONCE(shortcut->next_node);
	if (((level ^ sc_level) & ~ASSOC_ARRAY_KEY_CHUNK_MASK) != 0) {
		level = sc_level;
		goto jumped;
	} else {
		level = sc_level;
		goto consider_node;
	}
}