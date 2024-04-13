static int hdr_validate_tokens(struct crypt_device *cd, json_object *hdr_jobj)
{
	json_object *jobj;

	if (!json_object_object_get_ex(hdr_jobj, "tokens", &jobj)) {
		log_dbg(cd, "Missing tokens section.");
		return 1;
	}

	json_object_object_foreach(jobj, key, val) {
		if (!numbered(cd, "Token", key))
			return 1;
		if (LUKS2_token_validate(cd, hdr_jobj, val, key))
			return 1;
	}

	return 0;
}