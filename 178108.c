int LUKS2_segments_count(struct luks2_hdr *hdr)
{
	if (!hdr)
		return -EINVAL;

	return json_segments_count(LUKS2_get_segments_jobj(hdr));
}