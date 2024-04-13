static bool assoc_array_insert_mid_shortcut(struct assoc_array_edit *edit,
					    const struct assoc_array_ops *ops,
					    struct assoc_array_walk_result *result)
{
	struct assoc_array_shortcut *shortcut, *new_s0, *new_s1;
	struct assoc_array_node *node, *new_n0, *side;
	unsigned long sc_segments, dissimilarity, blank;
	size_t keylen;
	int level, sc_level, diff;
	int sc_slot;

	shortcut	= result->wrong_shortcut.shortcut;
	level		= result->wrong_shortcut.level;
	sc_level	= result->wrong_shortcut.sc_level;
	sc_segments	= result->wrong_shortcut.sc_segments;
	dissimilarity	= result->wrong_shortcut.dissimilarity;

	pr_devel("-->%s(ix=%d dis=%lx scix=%d)\n",
		 __func__, level, dissimilarity, sc_level);

	/* We need to split a shortcut and insert a node between the two
	 * pieces.  Zero-length pieces will be dispensed with entirely.
	 *
	 * First of all, we need to find out in which level the first
	 * difference was.
	 */
	diff = __ffs(dissimilarity);
	diff &= ~ASSOC_ARRAY_LEVEL_STEP_MASK;
	diff += sc_level & ~ASSOC_ARRAY_KEY_CHUNK_MASK;
	pr_devel("diff=%d\n", diff);

	if (!shortcut->back_pointer) {
		edit->set[0].ptr = &edit->array->root;
	} else if (assoc_array_ptr_is_node(shortcut->back_pointer)) {
		node = assoc_array_ptr_to_node(shortcut->back_pointer);
		edit->set[0].ptr = &node->slots[shortcut->parent_slot];
	} else {
		BUG();
	}

	edit->excised_meta[0] = assoc_array_shortcut_to_ptr(shortcut);

	/* Create a new node now since we're going to need it anyway */
	new_n0 = kzalloc(sizeof(struct assoc_array_node), GFP_KERNEL);
	if (!new_n0)
		return false;
	edit->new_meta[0] = assoc_array_node_to_ptr(new_n0);
	edit->adjust_count_on = new_n0;

	/* Insert a new shortcut before the new node if this segment isn't of
	 * zero length - otherwise we just connect the new node directly to the
	 * parent.
	 */
	level += ASSOC_ARRAY_LEVEL_STEP;
	if (diff > level) {
		pr_devel("pre-shortcut %d...%d\n", level, diff);
		keylen = round_up(diff, ASSOC_ARRAY_KEY_CHUNK_SIZE);
		keylen >>= ASSOC_ARRAY_KEY_CHUNK_SHIFT;

		new_s0 = kzalloc(sizeof(struct assoc_array_shortcut) +
				 keylen * sizeof(unsigned long), GFP_KERNEL);
		if (!new_s0)
			return false;
		edit->new_meta[1] = assoc_array_shortcut_to_ptr(new_s0);
		edit->set[0].to = assoc_array_shortcut_to_ptr(new_s0);
		new_s0->back_pointer = shortcut->back_pointer;
		new_s0->parent_slot = shortcut->parent_slot;
		new_s0->next_node = assoc_array_node_to_ptr(new_n0);
		new_s0->skip_to_level = diff;

		new_n0->back_pointer = assoc_array_shortcut_to_ptr(new_s0);
		new_n0->parent_slot = 0;

		memcpy(new_s0->index_key, shortcut->index_key,
		       keylen * sizeof(unsigned long));

		blank = ULONG_MAX << (diff & ASSOC_ARRAY_KEY_CHUNK_MASK);
		pr_devel("blank off [%zu] %d: %lx\n", keylen - 1, diff, blank);
		new_s0->index_key[keylen - 1] &= ~blank;
	} else {
		pr_devel("no pre-shortcut\n");
		edit->set[0].to = assoc_array_node_to_ptr(new_n0);
		new_n0->back_pointer = shortcut->back_pointer;
		new_n0->parent_slot = shortcut->parent_slot;
	}

	side = assoc_array_ptr_to_node(shortcut->next_node);
	new_n0->nr_leaves_on_branch = side->nr_leaves_on_branch;

	/* We need to know which slot in the new node is going to take a
	 * metadata pointer.
	 */
	sc_slot = sc_segments >> (diff & ASSOC_ARRAY_KEY_CHUNK_MASK);
	sc_slot &= ASSOC_ARRAY_FAN_MASK;

	pr_devel("new slot %lx >> %d -> %d\n",
		 sc_segments, diff & ASSOC_ARRAY_KEY_CHUNK_MASK, sc_slot);

	/* Determine whether we need to follow the new node with a replacement
	 * for the current shortcut.  We could in theory reuse the current
	 * shortcut if its parent slot number doesn't change - but that's a
	 * 1-in-16 chance so not worth expending the code upon.
	 */
	level = diff + ASSOC_ARRAY_LEVEL_STEP;
	if (level < shortcut->skip_to_level) {
		pr_devel("post-shortcut %d...%d\n", level, shortcut->skip_to_level);
		keylen = round_up(shortcut->skip_to_level, ASSOC_ARRAY_KEY_CHUNK_SIZE);
		keylen >>= ASSOC_ARRAY_KEY_CHUNK_SHIFT;

		new_s1 = kzalloc(sizeof(struct assoc_array_shortcut) +
				 keylen * sizeof(unsigned long), GFP_KERNEL);
		if (!new_s1)
			return false;
		edit->new_meta[2] = assoc_array_shortcut_to_ptr(new_s1);

		new_s1->back_pointer = assoc_array_node_to_ptr(new_n0);
		new_s1->parent_slot = sc_slot;
		new_s1->next_node = shortcut->next_node;
		new_s1->skip_to_level = shortcut->skip_to_level;

		new_n0->slots[sc_slot] = assoc_array_shortcut_to_ptr(new_s1);

		memcpy(new_s1->index_key, shortcut->index_key,
		       keylen * sizeof(unsigned long));

		edit->set[1].ptr = &side->back_pointer;
		edit->set[1].to = assoc_array_shortcut_to_ptr(new_s1);
	} else {
		pr_devel("no post-shortcut\n");

		/* We don't have to replace the pointed-to node as long as we
		 * use memory barriers to make sure the parent slot number is
		 * changed before the back pointer (the parent slot number is
		 * irrelevant to the old parent shortcut).
		 */
		new_n0->slots[sc_slot] = shortcut->next_node;
		edit->set_parent_slot[0].p = &side->parent_slot;
		edit->set_parent_slot[0].to = sc_slot;
		edit->set[1].ptr = &side->back_pointer;
		edit->set[1].to = assoc_array_node_to_ptr(new_n0);
	}

	/* Install the new leaf in a spare slot in the new node. */
	if (sc_slot == 0)
		edit->leaf_p = &new_n0->slots[1];
	else
		edit->leaf_p = &new_n0->slots[0];

	pr_devel("<--%s() = ok [split shortcut]\n", __func__);
	return edit;
}