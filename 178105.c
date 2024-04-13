int LUKS2_get_sector_size(struct luks2_hdr *hdr)
{
	json_object *jobj_segment;

	jobj_segment = LUKS2_get_segment_jobj(hdr, CRYPT_DEFAULT_SEGMENT);
	if (!jobj_segment)
		return SECTOR_SIZE;

	return json_segment_get_sector_size(jobj_segment) ?: SECTOR_SIZE;
}