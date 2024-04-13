static uint64_t LUKS2_metadata_size_jobj(json_object *jobj)
{
	json_object *jobj1, *jobj2;
	uint64_t json_size;

	json_object_object_get_ex(jobj, "config", &jobj1);
	json_object_object_get_ex(jobj1, "json_size", &jobj2);
	json_str_to_uint64(jobj2, &json_size);

	return json_size + LUKS2_HDR_BIN_LEN;
}