struct assoc_array_edit *assoc_array_clear(struct assoc_array *array,
					   const struct assoc_array_ops *ops)
{
	struct assoc_array_edit *edit;

	pr_devel("-->%s()\n", __func__);

	if (!array->root)
		return NULL;

	edit = kzalloc(sizeof(struct assoc_array_edit), GFP_KERNEL);
	if (!edit)
		return ERR_PTR(-ENOMEM);
	edit->array = array;
	edit->ops = ops;
	edit->set[1].ptr = &array->root;
	edit->set[1].to = NULL;
	edit->excised_subtree = array->root;
	edit->ops_for_excised_subtree = ops;
	pr_devel("all gone\n");
	return edit;
}