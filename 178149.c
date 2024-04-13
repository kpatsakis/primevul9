static int hdr_cleanup_and_validate(struct crypt_device *cd, struct luks2_hdr *hdr)
{
	LUKS2_digests_erase_unused(cd, hdr);

	return LUKS2_hdr_validate(cd, hdr->jobj, hdr->hdr_size - LUKS2_HDR_BIN_LEN);
}