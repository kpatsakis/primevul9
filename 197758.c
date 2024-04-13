const char *blkid_partition_get_name(blkid_partition par)
{
	return *par->name ? (char *) par->name : NULL;
}