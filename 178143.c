static int hdr_validate_json_size(struct crypt_device *cd, json_object *hdr_jobj, uint64_t hdr_json_size)
{
	json_object *jobj, *jobj1;
	const char *json;
	uint64_t json_area_size, json_size;

	json_object_object_get_ex(hdr_jobj, "config", &jobj);
	json_object_object_get_ex(jobj, "json_size", &jobj1);

	json = json_object_to_json_string_ext(hdr_jobj,
		JSON_C_TO_STRING_PLAIN | JSON_C_TO_STRING_NOSLASHESCAPE);
	json_area_size = crypt_jobj_get_uint64(jobj1);
	json_size = (uint64_t)strlen(json);

	if (hdr_json_size != json_area_size) {
		log_dbg(cd, "JSON area size does not match value in binary header.");
		return 1;
	}

	if (json_size > json_area_size) {
		log_dbg(cd, "JSON does not fit in the designated area.");
		return 1;
	}

	return 0;
}