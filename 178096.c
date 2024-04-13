json_object *LUKS2_get_digest_jobj(struct luks2_hdr *hdr, int digest)
{
	json_object *jobj1, *jobj2;
	char digest_name[16];

	if (!hdr || digest < 0)
		return NULL;

	if (snprintf(digest_name, sizeof(digest_name), "%u", digest) < 1)
		return NULL;

	if (!json_object_object_get_ex(hdr->jobj, "digests", &jobj1))
		return NULL;

	json_object_object_get_ex(jobj1, digest_name, &jobj2);
	return jobj2;
}