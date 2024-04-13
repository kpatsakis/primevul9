int blkid_parttable_set_id(blkid_parttable tab, const unsigned char *id)
{
	if (!tab)
		return -1;

	strncpy(tab->id, (const char *) id, sizeof(tab->id));
	return 0;
}