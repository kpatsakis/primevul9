static timelib_sll timelib_get_relative_text(char **ptr, int *behavior)
{
	while (**ptr == ' ' || **ptr == '\t' || **ptr == '-' || **ptr == '/') {
		++*ptr;
	}
	return timelib_lookup_relative_text(ptr, behavior);
}