static int LUKS2_keyslot_get_volume_key_size(struct luks2_hdr *hdr, const char *keyslot)
{
	json_object *jobj1, *jobj2, *jobj3;

	if (!json_object_object_get_ex(hdr->jobj, "keyslots", &jobj1))
		return -1;

	if (!json_object_object_get_ex(jobj1, keyslot, &jobj2))
		return -1;

	if (!json_object_object_get_ex(jobj2, "key_size", &jobj3))
		return -1;

	return json_object_get_int(jobj3);
}