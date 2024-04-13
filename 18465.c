static void crng_backtrack_protect(__u8 tmp[CHACHA_BLOCK_SIZE], int used)
{
	struct crng_state *crng = NULL;

#ifdef CONFIG_NUMA
	if (crng_node_pool)
		crng = crng_node_pool[numa_node_id()];
	if (crng == NULL)
#endif
		crng = &primary_crng;
	_crng_backtrack_protect(crng, tmp, used);
}