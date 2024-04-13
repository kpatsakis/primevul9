static bool assoc_array_insert_in_empty_tree(struct assoc_array_edit *edit)
{
	struct assoc_array_node *new_n0;

	pr_devel("-->%s()\n", __func__);

	new_n0 = kzalloc(sizeof(struct assoc_array_node), GFP_KERNEL);
	if (!new_n0)
		return false;

	edit->new_meta[0] = assoc_array_node_to_ptr(new_n0);
	edit->leaf_p = &new_n0->slots[0];
	edit->adjust_count_on = new_n0;
	edit->set[0].ptr = &edit->array->root;
	edit->set[0].to = assoc_array_node_to_ptr(new_n0);

	pr_devel("<--%s() = ok [no root]\n", __func__);
	return true;
}