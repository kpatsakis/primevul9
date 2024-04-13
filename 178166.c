json_object *LUKS2_get_tokens_jobj(struct luks2_hdr *hdr)
{
	json_object *jobj_tokens;

	if (!hdr || !json_object_object_get_ex(hdr->jobj, "tokens", &jobj_tokens))
		return NULL;

	return jobj_tokens;
}