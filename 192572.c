static int coolkey_get_init_and_get_count(list_t *list, int *countp)
{
	*countp = list_size(list);
	list_iterator_start(list);
	return SC_SUCCESS;
}