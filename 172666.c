static int get_raid56_logic_offset(u64 physical, int num,
				   struct map_lookup *map, u64 *offset,
				   u64 *stripe_start)
{
	int i;
	int j = 0;
	u64 stripe_nr;
	u64 last_offset;
	u32 stripe_index;
	u32 rot;

	last_offset = (physical - map->stripes[num].physical) *
		      nr_data_stripes(map);
	if (stripe_start)
		*stripe_start = last_offset;

	*offset = last_offset;
	for (i = 0; i < nr_data_stripes(map); i++) {
		*offset = last_offset + i * map->stripe_len;

		stripe_nr = div64_u64(*offset, map->stripe_len);
		stripe_nr = div_u64(stripe_nr, nr_data_stripes(map));

		/* Work out the disk rotation on this stripe-set */
		stripe_nr = div_u64_rem(stripe_nr, map->num_stripes, &rot);
		/* calculate which stripe this data locates */
		rot += i;
		stripe_index = rot % map->num_stripes;
		if (stripe_index == num)
			return 0;
		if (stripe_index < num)
			j++;
	}
	*offset = last_offset + j * map->stripe_len;
	return 1;
}