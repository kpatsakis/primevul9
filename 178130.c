void JSON_DBG(struct crypt_device *cd, json_object *jobj, const char *desc)
{
	if (desc)
		crypt_log(cd, CRYPT_LOG_DEBUG_JSON, desc);
	crypt_log(cd, CRYPT_LOG_DEBUG_JSON, json_object_to_json_string_ext(jobj,
		JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE));
}