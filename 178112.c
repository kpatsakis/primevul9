static void hdr_dump_tokens(struct crypt_device *cd, json_object *hdr_jobj)
{
	char token[16];
	json_object *tokens_jobj, *jobj2, *jobj3, *val;
	const char *tmps;
	int i, j;

	log_std(cd, "Tokens:\n");
	json_object_object_get_ex(hdr_jobj, "tokens", &tokens_jobj);

	for (j = 0; j < LUKS2_TOKENS_MAX; j++) {
		(void) snprintf(token, sizeof(token), "%i", j);
		json_object_object_get_ex(tokens_jobj, token, &val);
		if (!val)
			continue;

		json_object_object_get_ex(val, "type", &jobj2);
		tmps = json_object_get_string(jobj2);
		log_std(cd, "  %s: %s\n", token, tmps);

		LUKS2_token_dump(cd, j);

		json_object_object_get_ex(val, "keyslots", &jobj2);
		for (i = 0; i < (int) json_object_array_length(jobj2); i++) {
			jobj3 = json_object_array_get_idx(jobj2, i);
			log_std(cd, "\tKeyslot:    %s\n", json_object_get_string(jobj3));
		}
	}
}