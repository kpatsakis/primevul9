static int LUKS2_keyslot_validate(struct crypt_device *cd, json_object *hdr_jobj, json_object *hdr_keyslot, const char *key)
{
	json_object *jobj_key_size;

	if (!json_contains(cd, hdr_keyslot, key, "Keyslot", "type", json_type_string))
		return 1;
	if (!(jobj_key_size = json_contains(cd, hdr_keyslot, key, "Keyslot", "key_size", json_type_int)))
		return 1;

	/* enforce uint32_t type */
	if (!validate_json_uint32(jobj_key_size)) {
		log_dbg(cd, "Illegal field \"key_size\":%s.",
			json_object_get_string(jobj_key_size));
		return 1;
	}

	return 0;
}