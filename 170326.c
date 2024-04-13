struct bpf_map *bpf_map_inc_not_zero(struct bpf_map *map)
{
	spin_lock_bh(&map_idr_lock);
	map = __bpf_map_inc_not_zero(map, false);
	spin_unlock_bh(&map_idr_lock);

	return map;
}