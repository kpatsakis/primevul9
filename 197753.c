int blkid_parttable_set_uuid(blkid_parttable tab, const unsigned char *id)
{
	if (!tab)
		return -1;

	blkid_unparse_uuid(id, tab->id, sizeof(tab->id));
	return 0;
}