int LUKS2_check_json_size(struct crypt_device *cd, const struct luks2_hdr *hdr)
{
	return hdr_validate_json_size(cd, hdr->jobj, hdr->hdr_size - LUKS2_HDR_BIN_LEN);
}