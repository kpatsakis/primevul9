const char *LUKS2_get_keyslot_cipher(struct luks2_hdr *hdr, int keyslot, size_t *key_size)
{
	json_object *jobj_keyslot, *jobj_area, *jobj1;

	jobj_keyslot = LUKS2_get_keyslot_jobj(hdr, keyslot);
	if (!jobj_keyslot)
		return NULL;

	if (!json_object_object_get_ex(jobj_keyslot, "area", &jobj_area))
		return NULL;

	/* currently we only support raw length preserving area encryption */
	json_object_object_get_ex(jobj_area, "type", &jobj1);
	if (strcmp(json_object_get_string(jobj1), "raw"))
		return NULL;

	if (!json_object_object_get_ex(jobj_area, "key_size", &jobj1))
		return NULL;
	*key_size = json_object_get_int(jobj1);

	if (!json_object_object_get_ex(jobj_area, "encryption", &jobj1))
		return NULL;

	return json_object_get_string(jobj1);
}