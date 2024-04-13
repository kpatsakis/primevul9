static void extract_crng(__u8 out[CHACHA_BLOCK_SIZE])
{
	struct crng_state *crng = NULL;

#ifdef CONFIG_NUMA
	if (crng_node_pool)
		crng = crng_node_pool[numa_node_id()];
	if (crng == NULL)
#endif
		crng = &primary_crng;
	_extract_crng(crng, out);
}