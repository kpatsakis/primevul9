static int blkid_partitions_probe_partition(blkid_probe pr)
{
	blkid_probe disk_pr = NULL;
	blkid_partlist ls;
	blkid_partition par;
	dev_t devno;

	DBG(LOWPROBE, ul_debug("parts: start probing for partition entry"));

	if (pr->flags & BLKID_FL_NOSCAN_DEV)
		goto nothing;

	devno = blkid_probe_get_devno(pr);
	if (!devno)
		goto nothing;

	disk_pr = blkid_probe_get_wholedisk_probe(pr);
	if (!disk_pr)
		goto nothing;

	/* parse PT */
	ls = blkid_probe_get_partitions(disk_pr);
	if (!ls)
		goto nothing;

	par = blkid_partlist_devno_to_partition(ls, devno);
	if (!par)
		goto nothing;
	else {
		const char *v;
		blkid_parttable tab = blkid_partition_get_table(par);
		dev_t disk = blkid_probe_get_devno(disk_pr);

		if (tab) {
			v = blkid_parttable_get_type(tab);
			if (v)
				blkid_probe_set_value(pr, "PART_ENTRY_SCHEME",
					(unsigned char *) v, strlen(v) + 1);
		}

		v = blkid_partition_get_name(par);
		if (v)
			blkid_probe_set_value(pr, "PART_ENTRY_NAME",
				(unsigned char *) v, strlen(v) + 1);

		v = blkid_partition_get_uuid(par);
		if (v)
			blkid_probe_set_value(pr, "PART_ENTRY_UUID",
				(unsigned char *) v, strlen(v) + 1);

		/* type */
		v = blkid_partition_get_type_string(par);
		if (v)
			blkid_probe_set_value(pr, "PART_ENTRY_TYPE",
				(unsigned char *) v, strlen(v) + 1);
		else
			blkid_probe_sprintf_value(pr, "PART_ENTRY_TYPE",
				"0x%x", blkid_partition_get_type(par));

		if (blkid_partition_get_flags(par))
			blkid_probe_sprintf_value(pr, "PART_ENTRY_FLAGS",
				"0x%llx", blkid_partition_get_flags(par));

		blkid_probe_sprintf_value(pr, "PART_ENTRY_NUMBER",
				"%d", blkid_partition_get_partno(par));

		blkid_probe_sprintf_value(pr, "PART_ENTRY_OFFSET", "%jd",
				(intmax_t)blkid_partition_get_start(par));
		blkid_probe_sprintf_value(pr, "PART_ENTRY_SIZE", "%jd",
				(intmax_t)blkid_partition_get_size(par));

		blkid_probe_sprintf_value(pr, "PART_ENTRY_DISK", "%u:%u",
				major(disk), minor(disk));
	}

	DBG(LOWPROBE, ul_debug("parts: end probing for partition entry [success]"));
	return BLKID_PROBE_OK;

nothing:
	DBG(LOWPROBE, ul_debug("parts: end probing for partition entry [nothing]"));
	return BLKID_PROBE_NONE;


}