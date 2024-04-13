static void hdr_dump_digests(struct crypt_device *cd, json_object *hdr_jobj)
{
	char key[16];
	json_object *jobj1, *jobj2, *val;
	const char *tmps;
	int i;

	log_std(cd, "Digests:\n");
	json_object_object_get_ex(hdr_jobj, "digests", &jobj1);

	for (i = 0; i < LUKS2_DIGEST_MAX; i++) {
		(void) snprintf(key, sizeof(key), "%i", i);
		json_object_object_get_ex(jobj1, key, &val);
		if (!val)
			continue;

		json_object_object_get_ex(val, "type", &jobj2);
		tmps = json_object_get_string(jobj2);
		log_std(cd, "  %s: %s\n", key, tmps);

		LUKS2_digest_dump(cd, i);
	}
}