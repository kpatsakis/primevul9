struct json_object *LUKS2_array_jobj(struct json_object *array, const char *num)
{
	struct json_object *jobj1;
	int i;

	for (i = 0; i < (int) json_object_array_length(array); i++) {
		jobj1 = json_object_array_get_idx(array, i);
		if (!strcmp(num, json_object_get_string(jobj1)))
			return jobj1;
	}

	return NULL;
}