int LUKS2_hdr_validate(struct crypt_device *cd, json_object *hdr_jobj, uint64_t json_size)
{
	struct {
		int (*validate)(struct crypt_device *, json_object *);
	} checks[] = {
		{ hdr_validate_tokens   },
		{ hdr_validate_digests  },
		{ hdr_validate_segments },
		{ hdr_validate_keyslots },
		{ hdr_validate_config   },
		{ hdr_validate_areas    },
		{ NULL }
	};
	int i;

	if (!hdr_jobj)
		return 1;

	for (i = 0; checks[i].validate; i++)
		if (checks[i].validate && checks[i].validate(cd, hdr_jobj))
			return 1;

	if (hdr_validate_json_size(cd, hdr_jobj, json_size))
		return 1;

	/* validate keyslot implementations */
	if (LUKS2_keyslots_validate(cd, hdr_jobj))
		return 1;

	return 0;
}