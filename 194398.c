static int16_t findOffset(const char* const* list, const char* key)
{
	const char* const* anchor = list;
	while (*list != NULL) {
		if (strcmp(key, *list) == 0) {
			return (int16_t)(list - anchor);
		}
		list++;
	}

	return -1;

}