int LUKS2_get_default_segment(struct luks2_hdr *hdr)
{
	int s = LUKS2_get_segment_id_by_flag(hdr, "backup-final");
	if (s >= 0)
		return s;

	if (LUKS2_segments_count(hdr) == 1)
		return 0;

	return -EINVAL;
}