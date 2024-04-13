is_valid_string (MonoImage *image, guint32 str_index, int notnull)
{
	const char *p, *blob_end, *res;

	if (str_index >= image->heap_strings.size)
		return NULL;
	res = p = mono_metadata_string_heap (image, str_index);
	blob_end = mono_metadata_string_heap (image, image->heap_strings.size - 1);
	if (notnull && !*p)
		return 0;
	/* 
	 * FIXME: should check it's a valid utf8 string, too.
	 */
	while (p <= blob_end) {
		if (!*p)
			return res;
		++p;
	}
	return *p? NULL: res;
}