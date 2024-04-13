int LUKS2_deactivate(struct crypt_device *cd, const char *name, struct luks2_hdr *hdr, struct crypt_dm_active_device *dmd, uint32_t flags)
{
	int r, ret;
	struct dm_target *tgt;
	crypt_status_info ci;
	struct crypt_dm_active_device dmdc;
	char **dep, deps_uuid_prefix[40], *deps[MAX_DM_DEPS+1] = { 0 };
	const char *namei = NULL;
	struct crypt_lock_handle *reencrypt_lock = NULL;

	if (!dmd || !dmd->uuid || strncmp(CRYPT_LUKS2, dmd->uuid, sizeof(CRYPT_LUKS2)-1))
		return -EINVAL;

	/* uuid mismatch with metadata (if available) */
	if (hdr && crypt_uuid_cmp(dmd->uuid, hdr->uuid))
		return -EINVAL;

	r = snprintf(deps_uuid_prefix, sizeof(deps_uuid_prefix), CRYPT_SUBDEV "-%.32s", dmd->uuid + 6);
	if (r < 0 || (size_t)r != (sizeof(deps_uuid_prefix) - 1))
		return -EINVAL;

	tgt = &dmd->segment;

	/* TODO: We have LUKS2 dependencies now */
	if (hdr && single_segment(dmd) && tgt->type == DM_CRYPT && crypt_get_integrity_tag_size(cd))
		namei = device_dm_name(tgt->data_device);

	r = dm_device_deps(cd, name, deps_uuid_prefix, deps, ARRAY_SIZE(deps));
	if (r < 0)
		goto out;

	if (contains_reencryption_helper(deps)) {
		r = LUKS2_reencrypt_lock_by_dm_uuid(cd, dmd->uuid, &reencrypt_lock);
		if (r) {
			if (r == -EBUSY)
				log_err(cd, _("Reencryption in-progress. Cannot deactivate device."));
			else
				log_err(cd, _("Failed to get reencryption lock."));
			goto out;
		}
	}

	dep = deps;
	while (*dep) {
		if (is_reencryption_helper(*dep) && (dm_status_suspended(cd, *dep) > 0)) {
			if (dm_error_device(cd, *dep))
				log_err(cd, _("Failed to replace suspended device %s with dm-error target."), *dep);
		}
		dep++;
	}

	r = dm_query_device(cd, name, DM_ACTIVE_CRYPT_KEY | DM_ACTIVE_CRYPT_KEYSIZE, &dmdc);
	if (r < 0) {
		memset(&dmdc, 0, sizeof(dmdc));
		dmdc.segment.type = DM_UNKNOWN;
	}

	/* Remove top level device first */
	r = dm_remove_device(cd, name, flags);
	if (!r) {
		tgt = &dmdc.segment;
		while (tgt) {
			if (tgt->type == DM_CRYPT)
				crypt_drop_keyring_key_by_description(cd, tgt->u.crypt.vk->key_description, LOGON_KEY);
			tgt = tgt->next;
		}
	}
	dm_targets_free(cd, &dmdc);

	/* TODO: We have LUKS2 dependencies now */
	if (r >= 0 && namei) {
		log_dbg(cd, "Deactivating integrity device %s.", namei);
		r = dm_remove_device(cd, namei, 0);
	}

	if (!r) {
		ret = 0;
		dep = deps;
		while (*dep) {
			log_dbg(cd, "Deactivating LUKS2 dependent device %s.", *dep);
			r = dm_query_device(cd, *dep, DM_ACTIVE_CRYPT_KEY | DM_ACTIVE_CRYPT_KEYSIZE, &dmdc);
			if (r < 0) {
				memset(&dmdc, 0, sizeof(dmdc));
				dmdc.segment.type = DM_UNKNOWN;
			}

			r = dm_remove_device(cd, *dep, flags);
			if (r < 0) {
				ci = crypt_status(cd, *dep);
				if (ci == CRYPT_BUSY)
					log_err(cd, _("Device %s is still in use."), *dep);
				if (ci == CRYPT_INACTIVE)
					r = 0;
			}
			if (!r) {
				tgt = &dmdc.segment;
				while (tgt) {
					if (tgt->type == DM_CRYPT)
						crypt_drop_keyring_key_by_description(cd, tgt->u.crypt.vk->key_description, LOGON_KEY);
					tgt = tgt->next;
				}
			}
			dm_targets_free(cd, &dmdc);
			if (r && !ret)
				ret = r;
			dep++;
		}
		r = ret;
	}

out:
	LUKS2_reencrypt_unlock(cd, reencrypt_lock);
	dep = deps;
	while (*dep)
		free(*dep++);

	return r;
}