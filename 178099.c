int json_object_object_add_by_uint(json_object *jobj, unsigned key, json_object *jobj_val)
{
	char key_name[16];

	if (snprintf(key_name, sizeof(key_name), "%u", key) < 1)
		return -EINVAL;

#if HAVE_DECL_JSON_OBJECT_OBJECT_ADD_EX
	return json_object_object_add_ex(jobj, key_name, jobj_val, 0) ? -ENOMEM : 0;
#else
	json_object_object_add(jobj, key_name, jobj_val);
	return 0;
#endif
}