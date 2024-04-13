json_object *crypt_jobj_new_uint64(uint64_t value)
{
	/* 18446744073709551615 */
	char num[21];
	int r;
	json_object *jobj;

	r = snprintf(num, sizeof(num), "%" PRIu64, value);
	if (r < 0 || (size_t)r >= sizeof(num))
		return NULL;

	jobj = json_object_new_string(num);
	return jobj;
}