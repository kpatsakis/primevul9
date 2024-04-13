static void assoc_array_rcu_cleanup(struct rcu_head *head)
{
	struct assoc_array_edit *edit =
		container_of(head, struct assoc_array_edit, rcu);
	int i;

	pr_devel("-->%s()\n", __func__);

	if (edit->dead_leaf)
		edit->ops->free_object(assoc_array_ptr_to_leaf(edit->dead_leaf));
	for (i = 0; i < ARRAY_SIZE(edit->excised_meta); i++)
		if (edit->excised_meta[i])
			kfree(assoc_array_ptr_to_node(edit->excised_meta[i]));

	if (edit->excised_subtree) {
		BUG_ON(assoc_array_ptr_is_leaf(edit->excised_subtree));
		if (assoc_array_ptr_is_node(edit->excised_subtree)) {
			struct assoc_array_node *n =
				assoc_array_ptr_to_node(edit->excised_subtree);
			n->back_pointer = NULL;
		} else {
			struct assoc_array_shortcut *s =
				assoc_array_ptr_to_shortcut(edit->excised_subtree);
			s->back_pointer = NULL;
		}
		assoc_array_destroy_subtree(edit->excised_subtree,
					    edit->ops_for_excised_subtree);
	}

	kfree(edit);
}