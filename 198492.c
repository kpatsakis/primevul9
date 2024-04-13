static int cac_add_object_to_list(list_t *list, const cac_object_t *object)
{
	if (list_append(list, object) < 0)
		return SC_ERROR_UNKNOWN;
	return SC_SUCCESS;
}