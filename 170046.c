static timelib_long timelib_get_month(char **ptr)
{
	while (**ptr == ' ' || **ptr == '\t' || **ptr == '-' || **ptr == '.' || **ptr == '/') {
		++*ptr;
	}
	return timelib_lookup_month(ptr);
}