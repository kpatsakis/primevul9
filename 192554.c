static int coolkey_fetch_object(list_t *list, sc_cardctl_coolkey_object_t *coolkey_obj)
{
	sc_cardctl_coolkey_object_t *ptr;
	if (!list_iterator_hasnext(list)) {
		return SC_ERROR_FILE_END_REACHED;
	}

	ptr = list_iterator_next(list);
	*coolkey_obj = *ptr;
	return SC_SUCCESS;
}