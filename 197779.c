int blkid_partition_set_name(blkid_partition par,
		const unsigned char *name, size_t len)
{
	if (!par)
		return -1;

	set_string(par->name, sizeof(par->name), name, len);
	return 0;
}