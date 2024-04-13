int LUKS2_activate(struct crypt_device *cd,
	const char *name,
	struct volume_key *vk,
	uint32_t flags)
{
	int r;
	struct luks2_hdr *hdr = crypt_get_hdr(cd, CRYPT_LUKS2);
	struct crypt_dm_active_device dmdi = {}, dmd = {
		.uuid   = crypt_get_uuid(cd)
	};

	/* do not allow activation when particular requirements detected */
	if ((r = LUKS2_unmet_requirements(cd, hdr, 0, 0)))
		return r;

	r = dm_crypt_target_set(&dmd.segment, 0, dmd.size, crypt_data_device(cd),
			vk, crypt_get_cipher_spec(cd), crypt_get_iv_offset(cd),
			crypt_get_data_offset(cd), crypt_get_integrity(cd) ?: "none",
			crypt_get_integrity_tag_size(cd), crypt_get_sector_size(cd));
	if (r < 0)
		return r;

	/* Add persistent activation flags */
	if (!(flags & CRYPT_ACTIVATE_IGNORE_PERSISTENT))
		LUKS2_config_get_flags(cd, hdr, &dmd.flags);

	dmd.flags |= flags;

	if (crypt_get_integrity_tag_size(cd)) {
		if (!LUKS2_integrity_compatible(hdr)) {
			log_err(cd, _("Unsupported device integrity configuration."));
			return -EINVAL;
		}

		if (dmd.flags & CRYPT_ACTIVATE_ALLOW_DISCARDS) {
			log_err(cd, _("Discard/TRIM is not supported."));
			return -EINVAL;
		}

		r = INTEGRITY_create_dmd_device(cd, NULL, NULL, NULL, NULL, &dmdi, dmd.flags, 0);
		if (r)
			return r;

		dmdi.flags |= CRYPT_ACTIVATE_PRIVATE;
		dmdi.uuid = dmd.uuid;
		dmd.segment.u.crypt.offset = 0;
		dmd.segment.size = dmdi.segment.size;

		r = create_or_reload_device_with_integrity(cd, name, CRYPT_LUKS2, &dmd, &dmdi);
	} else
		r = create_or_reload_device(cd, name, CRYPT_LUKS2, &dmd);

	dm_targets_free(cd, &dmd);
	dm_targets_free(cd, &dmdi);

	return r;
}