static int hdr_validate_crypt_segment(struct crypt_device *cd,
				      json_object *jobj, const char *key, json_object *jobj_digests,
	uint64_t offset, uint64_t size)
{
	json_object *jobj_ivoffset, *jobj_sector_size, *jobj_integrity;
	uint32_t sector_size;
	uint64_t ivoffset;

	if (!(jobj_ivoffset = json_contains(cd, jobj, key, "Segment", "iv_tweak", json_type_string)) ||
	    !json_contains(cd, jobj, key, "Segment", "encryption", json_type_string) ||
	    !(jobj_sector_size = json_contains(cd, jobj, key, "Segment", "sector_size", json_type_int)))
		return 1;

	/* integrity */
	if (json_object_object_get_ex(jobj, "integrity", &jobj_integrity)) {
		if (!json_contains(cd, jobj, key, "Segment", "integrity", json_type_object) ||
		    !json_contains(cd, jobj_integrity, key, "Segment integrity", "type", json_type_string) ||
		    !json_contains(cd, jobj_integrity, key, "Segment integrity", "journal_encryption", json_type_string) ||
		    !json_contains(cd, jobj_integrity, key, "Segment integrity", "journal_integrity", json_type_string))
			return 1;
	}

	/* enforce uint32_t type */
	if (!validate_json_uint32(jobj_sector_size)) {
		log_dbg(cd, "Illegal field \"sector_size\":%s.",
			json_object_get_string(jobj_sector_size));
		return 1;
	}

	sector_size = crypt_jobj_get_uint32(jobj_sector_size);
	if (!sector_size || MISALIGNED_512(sector_size)) {
		log_dbg(cd, "Illegal sector size: %" PRIu32, sector_size);
		return 1;
	}

	if (!numbered(cd, "iv_tweak", json_object_get_string(jobj_ivoffset)) ||
	    !json_str_to_uint64(jobj_ivoffset, &ivoffset)) {
		log_dbg(cd, "Illegal iv_tweak value.");
		return 1;
	}

	if (size % sector_size) {
		log_dbg(cd, "Size field has to be aligned to sector size: %" PRIu32, sector_size);
		return 1;
	}

	return !segment_has_digest(key, jobj_digests);
}