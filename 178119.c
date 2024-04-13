int LUKS2_activate_multi(struct crypt_device *cd,
	const char *name,
	struct volume_key *vks,
	uint64_t device_size,
	uint32_t flags)
{
	struct luks2_hdr *hdr = crypt_get_hdr(cd, CRYPT_LUKS2);
	json_object *jobj_segments = LUKS2_get_segments_jobj(hdr);
	int r;
	struct crypt_dm_active_device dmd = {
		.size	= device_size,
		.uuid   = crypt_get_uuid(cd)
	};

	/* do not allow activation when particular requirements detected */
	if ((r = LUKS2_unmet_requirements(cd, hdr, CRYPT_REQUIREMENT_ONLINE_REENCRYPT, 0)))
		return r;

	/* Add persistent activation flags */
	if (!(flags & CRYPT_ACTIVATE_IGNORE_PERSISTENT))
		LUKS2_config_get_flags(cd, hdr, &dmd.flags);

	dmd.flags |= flags;

	r = LUKS2_assembly_multisegment_dmd(cd, hdr, vks, jobj_segments, &dmd);
	if (!r)
		r = dm_create_device(cd, name, CRYPT_LUKS2, &dmd);

	dm_targets_free(cd, &dmd);
	return r;
}