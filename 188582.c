void assoc_array_insert_set_object(struct assoc_array_edit *edit, void *object)
{
	BUG_ON(!object);
	edit->leaf = assoc_array_leaf_to_ptr(object);
}