json_object *LUKS2_get_token_jobj(struct luks2_hdr *hdr, int token)
{
	json_object *jobj1, *jobj2;
	char token_name[16];

	if (!hdr || token < 0)
		return NULL;

	jobj1 = LUKS2_get_tokens_jobj(hdr);
	if (!jobj1)
		return NULL;

	if (snprintf(token_name, sizeof(token_name), "%u", token) < 1)
		return NULL;

	json_object_object_get_ex(jobj1, token_name, &jobj2);
	return jobj2;
}