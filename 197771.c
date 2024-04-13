int blkid_partitions_need_typeonly(blkid_probe pr)
{
	struct blkid_chain *chn = blkid_probe_get_chain(pr);

	return chn && chn->data && chn->binary ? FALSE : TRUE;
}