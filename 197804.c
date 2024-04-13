blkid_partition blkid_partlist_devno_to_partition(blkid_partlist ls, dev_t devno)
{
	struct sysfs_cxt sysfs;
	uint64_t start, size;
	int i, rc, partno = 0;

	DBG(LOWPROBE, ul_debug("trying to convert devno 0x%llx to partition",
			(long long) devno));

	if (sysfs_init(&sysfs, devno, NULL)) {
		DBG(LOWPROBE, ul_debug("failed t init sysfs context"));
		return NULL;
	}
	rc = sysfs_read_u64(&sysfs, "size", &size);
	if (!rc) {
		rc = sysfs_read_u64(&sysfs, "start", &start);
		if (rc) {
			/* try to get partition number from DM uuid.
			 */
			char *uuid = sysfs_strdup(&sysfs, "dm/uuid");
			char *tmp = uuid;
			char *prefix = uuid ? strsep(&tmp, "-") : NULL;

			if (prefix && strncasecmp(prefix, "part", 4) == 0) {
				char *end = NULL;

				partno = strtol(prefix + 4, &end, 10);
				if (prefix == end || (end && *end))
					partno = 0;
				else
					rc = 0;		/* success */
			}
			free(uuid);
		}
	}

	sysfs_deinit(&sysfs);

	if (rc)
		return NULL;

	if (partno) {
		DBG(LOWPROBE, ul_debug("mapped by DM, using partno %d", partno));

		/*
		 * Partition mapped by kpartx does not provide "start" offset
		 * in /sys, but if we know partno and size of the partition
		 * that we can probably make the relation between the device
		 * and an entry in partition table.
		 */
		 for (i = 0; i < ls->nparts; i++) {
			 blkid_partition par = &ls->parts[i];

			 if (partno != blkid_partition_get_partno(par))
				 continue;

			 if (size == (uint64_t)blkid_partition_get_size(par) ||
			     (blkid_partition_is_extended(par) && size <= 1024ULL))
				 return par;

		 }
		 return NULL;
	}

	DBG(LOWPROBE, ul_debug("searching by offset/size"));

	for (i = 0; i < ls->nparts; i++) {
		blkid_partition par = &ls->parts[i];

		if ((uint64_t)blkid_partition_get_start(par) == start &&
		    (uint64_t)blkid_partition_get_size(par) == size)
			return par;

		/* exception for extended dos partitions */
		if ((uint64_t)blkid_partition_get_start(par) == start &&
		    blkid_partition_is_extended(par) && size <= 1024ULL)
			return par;

	}

	DBG(LOWPROBE, ul_debug("not found partition for device"));
	return NULL;
}