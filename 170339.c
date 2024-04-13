struct bpf_map *bpf_map_get_curr_or_next(u32 *id)
{
	struct bpf_map *map;

	spin_lock_bh(&map_idr_lock);
again:
	map = idr_get_next(&map_idr, id);
	if (map) {
		map = __bpf_map_inc_not_zero(map, false);
		if (IS_ERR(map)) {
			(*id)++;
			goto again;
		}
	}
	spin_unlock_bh(&map_idr_lock);

	return map;
}