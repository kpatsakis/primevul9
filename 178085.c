static json_bool json_str_to_uint64(json_object *jobj, uint64_t *value)
{
	char *endptr;
	unsigned long long tmp;

	errno = 0;
	tmp = strtoull(json_object_get_string(jobj), &endptr, 10);
	if (*endptr || errno) {
		*value = 0;
		return 0;
	}

	*value = tmp;
	return 1;
}