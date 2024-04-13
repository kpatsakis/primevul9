int blkid_partitions_get_flags(blkid_probe pr)
{
	struct blkid_chain *chn = blkid_probe_get_chain(pr);

	return chn ? chn->flags : 0;
}