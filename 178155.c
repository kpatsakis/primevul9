json_object *LUKS2_get_segments_jobj(struct luks2_hdr *hdr)
{
	return hdr ? json_get_segments_jobj(hdr->jobj) : NULL;
}