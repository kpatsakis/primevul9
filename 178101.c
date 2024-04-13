uint64_t LUKS2_metadata_size(struct luks2_hdr *hdr)
{
	return LUKS2_metadata_size_jobj(hdr->jobj);
}