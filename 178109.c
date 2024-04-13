json_object *LUKS2_get_segment_jobj(struct luks2_hdr *hdr, int segment)
{
	if (!hdr)
		return NULL;

	if (segment == CRYPT_DEFAULT_SEGMENT)
		segment = LUKS2_get_default_segment(hdr);

	return json_segments_get_segment(json_get_segments_jobj(hdr->jobj), segment);
}