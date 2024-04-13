uint32_t crypt_jobj_get_uint32(json_object *jobj)
{
	return json_object_get_int64(jobj);
}