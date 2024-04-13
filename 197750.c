static blkid_partlist partitions_init_data(struct blkid_chain *chn)
{
	blkid_partlist ls;

	if (chn->data)
		ls = (blkid_partlist) chn->data;
	else {
		/* allocate the new list of partitions */
		ls = calloc(1, sizeof(struct blkid_struct_partlist));
		if (!ls)
			return NULL;
		chn->data = (void *) ls;
	}

	reset_partlist(ls);

	DBG(LOWPROBE, ul_debug("parts: initialized partitions list (%p, size=%d)",
		ls, ls->nparts_max));
	return ls;
}