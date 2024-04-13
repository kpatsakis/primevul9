static void blkid_probe_set_partlist(blkid_probe pr, blkid_partlist ls)
{
	pr->chains[BLKID_CHAIN_PARTS].data = ls;
}