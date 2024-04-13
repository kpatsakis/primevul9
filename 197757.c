int blkid_partitions_strcpy_ptuuid(blkid_probe pr, char *str)
{
	struct blkid_chain *chn = blkid_probe_get_chain(pr);

	if (chn->binary || !str || !*str)
		return 0;

	if (!blkid_probe_set_value(pr, "PTUUID", (unsigned char *) str, strlen(str) + 1))
		return -ENOMEM;

	return 0;
}