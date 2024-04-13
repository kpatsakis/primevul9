static bool validate_segment_intervals(struct crypt_device *cd,
				    int length, const struct interval *ix)
{
	int j, i = 0;

	while (i < length) {
		if (ix[i].length == UINT64_MAX && (i != (length - 1))) {
			log_dbg(cd, "Only last regular segment is allowed to have 'dynamic' size.");
			return false;
		}

		for (j = 0; j < length; j++) {
			if (i == j)
				continue;
			if ((ix[i].offset >= ix[j].offset) && (ix[j].length == UINT64_MAX || (ix[i].offset < (ix[j].offset + ix[j].length)))) {
				log_dbg(cd, "Overlapping segments [%" PRIu64 ",%" PRIu64 "]%s and [%" PRIu64 ",%" PRIu64 "]%s.",
					ix[i].offset, ix[i].offset + ix[i].length, ix[i].length == UINT64_MAX ? "(dynamic)" : "",
					ix[j].offset, ix[j].offset + ix[j].length, ix[j].length == UINT64_MAX ? "(dynamic)" : "");
				return false;
			}
		}

		i++;
	}

	return true;
}