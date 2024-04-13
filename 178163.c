uint64_t crypt_jobj_get_uint64(json_object *jobj)
{
	uint64_t r;
	json_str_to_uint64(jobj, &r);
	return r;
}