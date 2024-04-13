const char *LUKS2_get_integrity(struct luks2_hdr *hdr, int segment)
{
	json_object *jobj1, *jobj2, *jobj3;

	jobj1 = LUKS2_get_segment_jobj(hdr, segment);
	if (!jobj1)
		return NULL;

	if (!json_object_object_get_ex(jobj1, "integrity", &jobj2))
		return NULL;

	if (!json_object_object_get_ex(jobj2, "type", &jobj3))
		return NULL;

	return json_object_get_string(jobj3);
}