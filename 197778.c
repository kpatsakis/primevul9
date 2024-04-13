int blkid_partlist_increment_partno(blkid_partlist ls)
{
	return ls ? ls->next_partno++ : -1;
}