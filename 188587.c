static void assoc_array_destroy_subtree(struct assoc_array_ptr *root,
					const struct assoc_array_ops *ops)
{
	struct assoc_array_shortcut *shortcut;
	struct assoc_array_node *node;
	struct assoc_array_ptr *cursor, *parent = NULL;
	int slot = -1;

	pr_devel("-->%s()\n", __func__);

	cursor = root;
	if (!cursor) {
		pr_devel("empty\n");
		return;
	}

move_to_meta:
	if (assoc_array_ptr_is_shortcut(cursor)) {
		/* Descend through a shortcut */
		pr_devel("[%d] shortcut\n", slot);
		BUG_ON(!assoc_array_ptr_is_shortcut(cursor));
		shortcut = assoc_array_ptr_to_shortcut(cursor);
		BUG_ON(shortcut->back_pointer != parent);
		BUG_ON(slot != -1 && shortcut->parent_slot != slot);
		parent = cursor;
		cursor = shortcut->next_node;
		slot = -1;
		BUG_ON(!assoc_array_ptr_is_node(cursor));
	}

	pr_devel("[%d] node\n", slot);
	node = assoc_array_ptr_to_node(cursor);
	BUG_ON(node->back_pointer != parent);
	BUG_ON(slot != -1 && node->parent_slot != slot);
	slot = 0;

continue_node:
	pr_devel("Node %p [back=%p]\n", node, node->back_pointer);
	for (; slot < ASSOC_ARRAY_FAN_OUT; slot++) {
		struct assoc_array_ptr *ptr = node->slots[slot];
		if (!ptr)
			continue;
		if (assoc_array_ptr_is_meta(ptr)) {
			parent = cursor;
			cursor = ptr;
			goto move_to_meta;
		}

		if (ops) {
			pr_devel("[%d] free leaf\n", slot);
			ops->free_object(assoc_array_ptr_to_leaf(ptr));
		}
	}

	parent = node->back_pointer;
	slot = node->parent_slot;
	pr_devel("free node\n");
	kfree(node);
	if (!parent)
		return; /* Done */

	/* Move back up to the parent (may need to free a shortcut on
	 * the way up) */
	if (assoc_array_ptr_is_shortcut(parent)) {
		shortcut = assoc_array_ptr_to_shortcut(parent);
		BUG_ON(shortcut->next_node != cursor);
		cursor = parent;
		parent = shortcut->back_pointer;
		slot = shortcut->parent_slot;
		pr_devel("free shortcut\n");
		kfree(shortcut);
		if (!parent)
			return;

		BUG_ON(!assoc_array_ptr_is_node(parent));
	}

	/* Ascend to next slot in parent node */
	pr_devel("ascend to %p[%d]\n", parent, slot);
	cursor = parent;
	node = assoc_array_ptr_to_node(cursor);
	slot++;
	goto continue_node;
}