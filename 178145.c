int LUKS2_assembly_multisegment_dmd(struct crypt_device *cd,
	struct luks2_hdr *hdr,
	struct volume_key *vks,
	json_object *jobj_segments,
	struct crypt_dm_active_device *dmd)
{
	struct volume_key *vk;
	json_object *jobj;
	enum devcheck device_check;
	int r;
	unsigned s = 0;
	uint64_t data_offset, segment_size, segment_offset, segment_start = 0;
	struct dm_target *t = &dmd->segment;

	if (dmd->flags & CRYPT_ACTIVATE_SHARED)
		device_check = DEV_OK;
	else
		device_check = DEV_EXCL;

	data_offset = LUKS2_reencrypt_data_offset(hdr, true);

	r = device_block_adjust(cd, crypt_data_device(cd), device_check,
			                                data_offset, &dmd->size, &dmd->flags);
	if (r)
		return r;

	r = dm_targets_allocate(&dmd->segment, json_segments_count(jobj_segments));
	if (r)
		goto err;

	r = -EINVAL;

	while (t) {
		jobj = json_segments_get_segment(jobj_segments, s);
		if (!jobj) {
			log_dbg(cd, "Internal error. Segment %u is null.", s);
			r = -EINVAL;
			goto err;
		}

		segment_offset = json_segment_get_offset(jobj, 1);
		segment_size = json_segment_get_size(jobj, 1);
		/* 'dynamic' length allowed in last segment only */
		if (!segment_size && !t->next)
			segment_size = dmd->size - segment_start;
		if (!segment_size) {
			log_dbg(cd, "Internal error. Wrong segment size %u", s);
			r = -EINVAL;
			goto err;
		}

		if (!strcmp(json_segment_type(jobj), "crypt")) {
			vk = crypt_volume_key_by_id(vks, LUKS2_digest_by_segment(hdr, s));
			if (!vk) {
				log_err(cd, _("Missing key for dm-crypt segment %u"), s);
				r = -EINVAL;
				goto err;
			}

			r = dm_crypt_target_set(t, segment_start, segment_size,
					crypt_data_device(cd), vk,
					json_segment_get_cipher(jobj),
					json_segment_get_iv_offset(jobj),
					segment_offset, "none", 0,
					json_segment_get_sector_size(jobj));
			if (r) {
				log_err(cd, _("Failed to set dm-crypt segment."));
				goto err;
			}
		} else if (!strcmp(json_segment_type(jobj), "linear")) {
			r = dm_linear_target_set(t, segment_start, segment_size, crypt_data_device(cd), segment_offset);
			if (r) {
				log_err(cd, _("Failed to set dm-linear segment."));
				goto err;
			}
		} else {
			r = -EINVAL;
			goto err;
		}

		segment_start += segment_size;
		t = t->next;
		s++;
	}

	return r;
err:
	dm_targets_free(cd, dmd);
	return r;
}