int LUKS2_config_set_flags(struct crypt_device *cd, struct luks2_hdr *hdr, uint32_t flags)
{
	json_object *jobj_config, *jobj_flags;
	int i;

	if (!json_object_object_get_ex(hdr->jobj, "config", &jobj_config))
		return 0;

	jobj_flags = json_object_new_array();

	for (i = 0; persistent_flags[i].description; i++) {
		if (flags & persistent_flags[i].flag) {
			log_dbg(cd, "Setting persistent flag: %s.", persistent_flags[i].description);
			json_object_array_add(jobj_flags,
				json_object_new_string(persistent_flags[i].description));
		}
	}

	/* Replace or add new flags array */
	json_object_object_add(jobj_config, "flags", jobj_flags);

	return LUKS2_hdr_write(cd, hdr);
}