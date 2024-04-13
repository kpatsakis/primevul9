int blkid_partition_set_type_uuid(blkid_partition par, const unsigned char *uuid)
{
	blkid_unparse_uuid(uuid, par->typestr, sizeof(par->typestr));
	return 0;
}