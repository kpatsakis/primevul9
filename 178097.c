const char *LUKS2_get_cipher(struct luks2_hdr *hdr, int segment)
{
	json_object *jobj_segment;

	if (!hdr)
		return NULL;

	if (segment == CRYPT_DEFAULT_SEGMENT)
		segment = LUKS2_get_default_segment(hdr);

	jobj_segment = json_segments_get_segment(json_get_segments_jobj(hdr->jobj), segment);
	if (!jobj_segment)
		return NULL;

	/* FIXME: default encryption (for other segment types) must be string here. */
	return json_segment_get_cipher(jobj_segment) ?: "null";
}