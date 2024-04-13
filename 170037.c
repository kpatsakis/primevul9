static timelib_sll timelib_get_nr(char **ptr, int max_length)
{
	return timelib_get_nr_ex(ptr, max_length, NULL);
}