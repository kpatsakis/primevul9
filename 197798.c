int blkid_partition_gen_uuid(blkid_partition par)
{
	if (!par || !par->tab || !*par->tab->id)
		return -1;

	snprintf(par->uuid, sizeof(par->uuid), "%s-%02x",
			par->tab->id, par->partno);
	return 0;
}