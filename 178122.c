static int hdr_validate_keyslots(struct crypt_device *cd, json_object *hdr_jobj)
{
	json_object *jobj;

	if (!json_object_object_get_ex(hdr_jobj, "keyslots", &jobj)) {
		log_dbg(cd, "Missing keyslots section.");
		return 1;
	}

	json_object_object_foreach(jobj, key, val) {
		if (!numbered(cd, "Keyslot", key))
			return 1;
		if (LUKS2_keyslot_validate(cd, hdr_jobj, val, key))
			return 1;
	}

	return 0;
}