static json_bool validate_intervals(struct crypt_device *cd,
				    int length, const struct interval *ix,
				    uint64_t metadata_size, uint64_t keyslots_area_end)
{
	int j, i = 0;

	while (i < length) {
		if (ix[i].offset < 2 * metadata_size) {
			log_dbg(cd, "Illegal area offset: %" PRIu64 ".", ix[i].offset);
			return 0;
		}

		if (!ix[i].length) {
			log_dbg(cd, "Area length must be greater than zero.");
			return 0;
		}

		if ((ix[i].offset + ix[i].length) > keyslots_area_end) {
			log_dbg(cd, "Area [%" PRIu64 ", %" PRIu64 "] overflows binary keyslots area (ends at offset: %" PRIu64 ").",
				ix[i].offset, ix[i].offset + ix[i].length, keyslots_area_end);
			return 0;
		}

		for (j = 0; j < length; j++) {
			if (i == j)
				continue;
			if ((ix[i].offset >= ix[j].offset) && (ix[i].offset < (ix[j].offset + ix[j].length))) {
				log_dbg(cd, "Overlapping areas [%" PRIu64 ",%" PRIu64 "] and [%" PRIu64 ",%" PRIu64 "].",
					ix[i].offset, ix[i].offset + ix[i].length,
					ix[j].offset, ix[j].offset + ix[j].length);
				return 0;
			}
		}

		i++;
	}

	return 1;
}