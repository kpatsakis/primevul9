void assoc_array_destroy(struct assoc_array *array,
			 const struct assoc_array_ops *ops)
{
	assoc_array_destroy_subtree(array->root, ops);
	array->root = NULL;
}