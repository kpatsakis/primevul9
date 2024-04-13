static void hdr_dump_config(struct crypt_device *cd, json_object *hdr_jobj)
{

	json_object *jobj1, *jobj_config, *jobj_flags, *jobj_requirements, *jobj_mandatory;
	int i = 0, flags = 0, reqs = 0;

	log_std(cd, "Flags:       \t");

	if (json_object_object_get_ex(hdr_jobj, "config", &jobj_config)) {
		if (json_object_object_get_ex(jobj_config, "flags", &jobj_flags))
			flags = (int) json_object_array_length(jobj_flags);
		if (json_object_object_get_ex(jobj_config, "requirements", &jobj_requirements) &&
		    json_object_object_get_ex(jobj_requirements, "mandatory", &jobj_mandatory))
			reqs = (int) json_object_array_length(jobj_mandatory);
	}

	for (i = 0; i < flags; i++) {
		jobj1 = json_object_array_get_idx(jobj_flags, i);
		log_std(cd, "%s ", json_object_get_string(jobj1));
	}

	log_std(cd, "%s\n%s", flags > 0 ? "" : "(no flags)", reqs > 0 ? "" : "\n");

	if (reqs > 0) {
		log_std(cd, "Requirements:\t");
		for (i = 0; i < reqs; i++) {
			jobj1 = json_object_array_get_idx(jobj_mandatory, i);
			log_std(cd, "%s ", json_object_get_string(jobj1));
		}
		log_std(cd, "\n\n");
	}
}