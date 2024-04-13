static int _reload_custom_multi(struct crypt_device *cd,
	const char *name,
	struct volume_key *vks,
	json_object *jobj_segments,
	uint64_t device_size,
	uint32_t flags)
{
	int r;
	struct luks2_hdr *hdr = crypt_get_hdr(cd, CRYPT_LUKS2);
	struct crypt_dm_active_device dmd =  {
		.uuid   = crypt_get_uuid(cd),
		.size = device_size >> SECTOR_SHIFT
	};

	/* do not allow activation when particular requirements detected */
	if ((r = LUKS2_unmet_requirements(cd, hdr, CRYPT_REQUIREMENT_ONLINE_REENCRYPT, 0)))
		return r;

	/* Add persistent activation flags */
	if (!(flags & CRYPT_ACTIVATE_IGNORE_PERSISTENT))
		LUKS2_config_get_flags(cd, hdr, &dmd.flags);

	dmd.flags |= (flags | CRYPT_ACTIVATE_SHARED);

	r = LUKS2_assembly_multisegment_dmd(cd, hdr, vks, jobj_segments, &dmd);
	if (!r)
		r = dm_reload_device(cd, name, &dmd, 0, 0);

	dm_targets_free(cd, &dmd);
	return r;
}