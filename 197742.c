blkid_partlist blkid_probe_get_partitions(blkid_probe pr)
{
	return (blkid_partlist) blkid_probe_get_binary_data(pr,
			&pr->chains[BLKID_CHAIN_PARTS]);
}