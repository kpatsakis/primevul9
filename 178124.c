crypt_reencrypt_info LUKS2_reencrypt_status(struct luks2_hdr *hdr)
{
	uint32_t reqs;

	/*
	 * Any unknown requirement or offline reencryption should abort
	 * anything related to online-reencryption handling
	 */
	if (LUKS2_config_get_requirements(NULL, hdr, &reqs))
		return CRYPT_REENCRYPT_INVALID;

	if (!reqs_reencrypt_online(reqs))
		return CRYPT_REENCRYPT_NONE;

	if (json_segments_segment_in_reencrypt(LUKS2_get_segments_jobj(hdr)) < 0)
		return CRYPT_REENCRYPT_CLEAN;

	return CRYPT_REENCRYPT_CRASH;
}