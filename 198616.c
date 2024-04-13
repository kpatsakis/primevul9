int sc_concatenate_path(sc_path_t *d, const sc_path_t *p1, const sc_path_t *p2)
{
	sc_path_t tpath;

	if (d == NULL || p1 == NULL || p2 == NULL)
		return SC_ERROR_INVALID_ARGUMENTS;

	if (p1->type == SC_PATH_TYPE_DF_NAME || p2->type == SC_PATH_TYPE_DF_NAME)
		/* we do not support concatenation of AIDs at the moment */
		return SC_ERROR_NOT_SUPPORTED;

	if (p1->len + p2->len > SC_MAX_PATH_SIZE)
		return SC_ERROR_INVALID_ARGUMENTS;

	memset(&tpath, 0, sizeof(sc_path_t));
	memcpy(tpath.value, p1->value, p1->len);
	memcpy(tpath.value + p1->len, p2->value, p2->len);
	tpath.len  = p1->len + p2->len;
	tpath.type = SC_PATH_TYPE_PATH;
	/* use 'index' and 'count' entry of the second path object */
	tpath.index = p2->index;
	tpath.count = p2->count;
	/* the result is currently always as path */
	tpath.type  = SC_PATH_TYPE_PATH;

	*d = tpath;

	return SC_SUCCESS;
}