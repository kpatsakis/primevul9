static int cac_get_init_and_get_count(list_t *list, cac_object_t **entry, int *countp)
{
	*countp = list_size(list);
	list_iterator_start(list);
	*entry = list_iterator_next(list);
	return SC_SUCCESS;
}