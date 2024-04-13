void json_object_object_del_by_uint(json_object *jobj, unsigned key)
{
	char key_name[16];

	if (snprintf(key_name, sizeof(key_name), "%u", key) < 1)
		return;
	json_object_object_del(jobj, key_name);
}