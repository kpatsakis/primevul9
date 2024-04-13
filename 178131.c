static uint64_t LUKS2_keyslots_size_jobj(json_object *jobj)
{
	json_object *jobj1, *jobj2;
	uint64_t keyslots_size;

	json_object_object_get_ex(jobj, "config", &jobj1);
	json_object_object_get_ex(jobj1, "keyslots_size", &jobj2);
	json_str_to_uint64(jobj2, &keyslots_size);

	return keyslots_size;
}