int LUKS2_config_get_flags(struct crypt_device *cd, struct luks2_hdr *hdr, uint32_t *flags)
{
	json_object *jobj1, *jobj_config, *jobj_flags;
	int i, j, found;

	if (!hdr || !flags)
		return -EINVAL;

	*flags = 0;

	if (!json_object_object_get_ex(hdr->jobj, "config", &jobj_config))
		return 0;

	if (!json_object_object_get_ex(jobj_config, "flags", &jobj_flags))
		return 0;

	for (i = 0; i < (int) json_object_array_length(jobj_flags); i++) {
		jobj1 = json_object_array_get_idx(jobj_flags, i);
		found = 0;
		for (j = 0; persistent_flags[j].description && !found; j++)
			if (!strcmp(persistent_flags[j].description,
				    json_object_get_string(jobj1))) {
				*flags |= persistent_flags[j].flag;
				log_dbg(cd, "Using persistent flag %s.",
					json_object_get_string(jobj1));
				found = 1;
			}
		if (!found)
			log_verbose(cd, _("Ignored unknown flag %s."),
				    json_object_get_string(jobj1));
	}

	return 0;
}