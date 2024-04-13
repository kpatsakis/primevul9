int LUKS2_reload(struct crypt_device *cd,
	const char *name,
	struct volume_key *vks,
	uint64_t device_size,
	uint32_t flags)
{
	if (crypt_get_integrity_tag_size(cd))
		return -ENOTSUP;

	return _reload_custom_multi(cd, name, vks,
			LUKS2_get_segments_jobj(crypt_get_hdr(cd, CRYPT_LUKS2)), device_size, flags);
}