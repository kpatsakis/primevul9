int blkid_partition_set_uuid(blkid_partition par, const unsigned char *uuid)
{
	if (!par)
		return -1;

	blkid_unparse_uuid(uuid, par->uuid, sizeof(par->uuid));
	return 0;
}