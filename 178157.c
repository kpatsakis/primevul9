int LUKS2_token_validate(struct crypt_device *cd,
			 json_object *hdr_jobj, json_object *jobj_token, const char *key)
{
	json_object *jarr, *jobj_keyslots;

	/* keyslots are not yet validated, but we need to know token doesn't reference missing keyslot */
	if (!json_object_object_get_ex(hdr_jobj, "keyslots", &jobj_keyslots))
		return 1;

	if (!json_contains(cd, jobj_token, key, "Token", "type", json_type_string))
		return 1;

	jarr = json_contains(cd, jobj_token, key, "Token", "keyslots", json_type_array);
	if (!jarr)
		return 1;

	if (!validate_keyslots_array(cd, jarr, jobj_keyslots))
		return 1;

	return 0;
}