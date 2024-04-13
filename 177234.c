static void sdma_populate_sde_map(struct sdma_rht_map_elem *map)
{
	int i;

	for (i = 0; i < roundup_pow_of_two(map->ctr ? : 1) - map->ctr; i++)
		map->sde[map->ctr + i] = map->sde[i];
}