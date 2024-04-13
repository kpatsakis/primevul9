void LUKS2_hdr_repair(struct crypt_device *cd, json_object *hdr_jobj)
{
	json_object *jobj_keyslots;

	if (!json_object_object_get_ex(hdr_jobj, "keyslots", &jobj_keyslots))
		return;
	if (!json_object_is_type(jobj_keyslots, json_type_object))
		return;

	LUKS2_keyslots_repair(cd, jobj_keyslots);
}