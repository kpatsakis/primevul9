uint64_t LUKS2_keyslots_size(struct luks2_hdr *hdr)
{
	return LUKS2_keyslots_size_jobj(hdr->jobj);
}