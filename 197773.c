blkid_partlist blkid_probe_get_partlist(blkid_probe pr)
{
	return (blkid_partlist) pr->chains[BLKID_CHAIN_PARTS].data;
}