void assoc_array_apply_edit(struct assoc_array_edit *edit)
{
	struct assoc_array_shortcut *shortcut;
	struct assoc_array_node *node;
	struct assoc_array_ptr *ptr;
	int i;

	pr_devel("-->%s()\n", __func__);

	smp_wmb();
	if (edit->leaf_p)
		*edit->leaf_p = edit->leaf;

	smp_wmb();
	for (i = 0; i < ARRAY_SIZE(edit->set_parent_slot); i++)
		if (edit->set_parent_slot[i].p)
			*edit->set_parent_slot[i].p = edit->set_parent_slot[i].to;

	smp_wmb();
	for (i = 0; i < ARRAY_SIZE(edit->set_backpointers); i++)
		if (edit->set_backpointers[i])
			*edit->set_backpointers[i] = edit->set_backpointers_to;

	smp_wmb();
	for (i = 0; i < ARRAY_SIZE(edit->set); i++)
		if (edit->set[i].ptr)
			*edit->set[i].ptr = edit->set[i].to;

	if (edit->array->root == NULL) {
		edit->array->nr_leaves_on_tree = 0;
	} else if (edit->adjust_count_on) {
		node = edit->adjust_count_on;
		for (;;) {
			node->nr_leaves_on_branch += edit->adjust_count_by;

			ptr = node->back_pointer;
			if (!ptr)
				break;
			if (assoc_array_ptr_is_shortcut(ptr)) {
				shortcut = assoc_array_ptr_to_shortcut(ptr);
				ptr = shortcut->back_pointer;
				if (!ptr)
					break;
			}
			BUG_ON(!assoc_array_ptr_is_node(ptr));
			node = assoc_array_ptr_to_node(ptr);
		}

		edit->array->nr_leaves_on_tree += edit->adjust_count_by;
	}

	call_rcu(&edit->rcu, assoc_array_rcu_cleanup);
}