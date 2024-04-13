uint64_t LUKS2_hdr_and_areas_size(struct luks2_hdr *hdr)
{
	return LUKS2_hdr_and_areas_size_jobj(hdr->jobj);
}