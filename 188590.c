struct assoc_array_edit *assoc_array_delete(struct assoc_array *array,
					    const struct assoc_array_ops *ops,
					    const void *index_key)
{
	struct assoc_array_delete_collapse_context collapse;
	struct assoc_array_walk_result result;
	struct assoc_array_node *node, *new_n0;
	struct assoc_array_edit *edit;
	struct assoc_array_ptr *ptr;
	bool has_meta;
	int slot, i;

	pr_devel("-->%s()\n", __func__);

	edit = kzalloc(sizeof(struct assoc_array_edit), GFP_KERNEL);
	if (!edit)
		return ERR_PTR(-ENOMEM);
	edit->array = array;
	edit->ops = ops;
	edit->adjust_count_by = -1;

	switch (assoc_array_walk(array, ops, index_key, &result)) {
	case assoc_array_walk_found_terminal_node:
		/* We found a node that should contain the leaf we've been
		 * asked to remove - *if* it's in the tree.
		 */
		pr_devel("terminal_node\n");
		node = result.terminal_node.node;

		for (slot = 0; slot < ASSOC_ARRAY_FAN_OUT; slot++) {
			ptr = node->slots[slot];
			if (ptr &&
			    assoc_array_ptr_is_leaf(ptr) &&
			    ops->compare_object(assoc_array_ptr_to_leaf(ptr),
						index_key))
				goto found_leaf;
		}
	case assoc_array_walk_tree_empty:
	case assoc_array_walk_found_wrong_shortcut:
	default:
		assoc_array_cancel_edit(edit);
		pr_devel("not found\n");
		return NULL;
	}

found_leaf:
	BUG_ON(array->nr_leaves_on_tree <= 0);

	/* In the simplest form of deletion we just clear the slot and release
	 * the leaf after a suitable interval.
	 */
	edit->dead_leaf = node->slots[slot];
	edit->set[0].ptr = &node->slots[slot];
	edit->set[0].to = NULL;
	edit->adjust_count_on = node;

	/* If that concludes erasure of the last leaf, then delete the entire
	 * internal array.
	 */
	if (array->nr_leaves_on_tree == 1) {
		edit->set[1].ptr = &array->root;
		edit->set[1].to = NULL;
		edit->adjust_count_on = NULL;
		edit->excised_subtree = array->root;
		pr_devel("all gone\n");
		return edit;
	}

	/* However, we'd also like to clear up some metadata blocks if we
	 * possibly can.
	 *
	 * We go for a simple algorithm of: if this node has FAN_OUT or fewer
	 * leaves in it, then attempt to collapse it - and attempt to
	 * recursively collapse up the tree.
	 *
	 * We could also try and collapse in partially filled subtrees to take
	 * up space in this node.
	 */
	if (node->nr_leaves_on_branch <= ASSOC_ARRAY_FAN_OUT + 1) {
		struct assoc_array_node *parent, *grandparent;
		struct assoc_array_ptr *ptr;

		/* First of all, we need to know if this node has metadata so
		 * that we don't try collapsing if all the leaves are already
		 * here.
		 */
		has_meta = false;
		for (i = 0; i < ASSOC_ARRAY_FAN_OUT; i++) {
			ptr = node->slots[i];
			if (assoc_array_ptr_is_meta(ptr)) {
				has_meta = true;
				break;
			}
		}

		pr_devel("leaves: %ld [m=%d]\n",
			 node->nr_leaves_on_branch - 1, has_meta);

		/* Look further up the tree to see if we can collapse this node
		 * into a more proximal node too.
		 */
		parent = node;
	collapse_up:
		pr_devel("collapse subtree: %ld\n", parent->nr_leaves_on_branch);

		ptr = parent->back_pointer;
		if (!ptr)
			goto do_collapse;
		if (assoc_array_ptr_is_shortcut(ptr)) {
			struct assoc_array_shortcut *s = assoc_array_ptr_to_shortcut(ptr);
			ptr = s->back_pointer;
			if (!ptr)
				goto do_collapse;
		}

		grandparent = assoc_array_ptr_to_node(ptr);
		if (grandparent->nr_leaves_on_branch <= ASSOC_ARRAY_FAN_OUT + 1) {
			parent = grandparent;
			goto collapse_up;
		}

	do_collapse:
		/* There's no point collapsing if the original node has no meta
		 * pointers to discard and if we didn't merge into one of that
		 * node's ancestry.
		 */
		if (has_meta || parent != node) {
			node = parent;

			/* Create a new node to collapse into */
			new_n0 = kzalloc(sizeof(struct assoc_array_node), GFP_KERNEL);
			if (!new_n0)
				goto enomem;
			edit->new_meta[0] = assoc_array_node_to_ptr(new_n0);

			new_n0->back_pointer = node->back_pointer;
			new_n0->parent_slot = node->parent_slot;
			new_n0->nr_leaves_on_branch = node->nr_leaves_on_branch;
			edit->adjust_count_on = new_n0;

			collapse.node = new_n0;
			collapse.skip_leaf = assoc_array_ptr_to_leaf(edit->dead_leaf);
			collapse.slot = 0;
			assoc_array_subtree_iterate(assoc_array_node_to_ptr(node),
						    node->back_pointer,
						    assoc_array_delete_collapse_iterator,
						    &collapse);
			pr_devel("collapsed %d,%lu\n", collapse.slot, new_n0->nr_leaves_on_branch);
			BUG_ON(collapse.slot != new_n0->nr_leaves_on_branch - 1);

			if (!node->back_pointer) {
				edit->set[1].ptr = &array->root;
			} else if (assoc_array_ptr_is_leaf(node->back_pointer)) {
				BUG();
			} else if (assoc_array_ptr_is_node(node->back_pointer)) {
				struct assoc_array_node *p =
					assoc_array_ptr_to_node(node->back_pointer);
				edit->set[1].ptr = &p->slots[node->parent_slot];
			} else if (assoc_array_ptr_is_shortcut(node->back_pointer)) {
				struct assoc_array_shortcut *s =
					assoc_array_ptr_to_shortcut(node->back_pointer);
				edit->set[1].ptr = &s->next_node;
			}
			edit->set[1].to = assoc_array_node_to_ptr(new_n0);
			edit->excised_subtree = assoc_array_node_to_ptr(node);
		}
	}

	return edit;

enomem:
	/* Clean up after an out of memory error */
	pr_devel("enomem\n");
	assoc_array_cancel_edit(edit);
	return ERR_PTR(-ENOMEM);
}