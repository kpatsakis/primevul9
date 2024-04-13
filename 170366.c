static int map_check_btf(struct bpf_map *map, const struct btf *btf,
			 u32 btf_key_id, u32 btf_value_id)
{
	const struct btf_type *key_type, *value_type;
	u32 key_size, value_size;
	int ret = 0;

	/* Some maps allow key to be unspecified. */
	if (btf_key_id) {
		key_type = btf_type_id_size(btf, &btf_key_id, &key_size);
		if (!key_type || key_size != map->key_size)
			return -EINVAL;
	} else {
		key_type = btf_type_by_id(btf, 0);
		if (!map->ops->map_check_btf)
			return -EINVAL;
	}

	value_type = btf_type_id_size(btf, &btf_value_id, &value_size);
	if (!value_type || value_size != map->value_size)
		return -EINVAL;

	map->spin_lock_off = btf_find_spin_lock(btf, value_type);

	if (map_value_has_spin_lock(map)) {
		if (map->map_flags & BPF_F_RDONLY_PROG)
			return -EACCES;
		if (map->map_type != BPF_MAP_TYPE_HASH &&
		    map->map_type != BPF_MAP_TYPE_ARRAY &&
		    map->map_type != BPF_MAP_TYPE_CGROUP_STORAGE &&
		    map->map_type != BPF_MAP_TYPE_SK_STORAGE &&
		    map->map_type != BPF_MAP_TYPE_INODE_STORAGE &&
		    map->map_type != BPF_MAP_TYPE_TASK_STORAGE)
			return -ENOTSUPP;
		if (map->spin_lock_off + sizeof(struct bpf_spin_lock) >
		    map->value_size) {
			WARN_ONCE(1,
				  "verifier bug spin_lock_off %d value_size %d\n",
				  map->spin_lock_off, map->value_size);
			return -EFAULT;
		}
	}

	if (map->ops->map_check_btf)
		ret = map->ops->map_check_btf(map, btf, key_type, value_type);

	return ret;
}