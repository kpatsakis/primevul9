static void sdma_cleanup_sde_map(struct sdma_rht_map_elem *map,
				 struct sdma_engine *sde)
{
	unsigned int i, pow;

	/* only need to check the first ctr entries for a match */
	for (i = 0; i < map->ctr; i++) {
		if (map->sde[i] == sde) {
			memmove(&map->sde[i], &map->sde[i + 1],
				(map->ctr - i - 1) * sizeof(map->sde[0]));
			map->ctr--;
			pow = roundup_pow_of_two(map->ctr ? : 1);
			map->mask = pow - 1;
			sdma_populate_sde_map(map);
			break;
		}
	}
}