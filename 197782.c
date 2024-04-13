int blkid_partlist_set_partno(blkid_partlist ls, int partno)
{
	if (!ls)
		return -1;
	ls->next_partno = partno;
	return 0;
}