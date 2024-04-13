static int bpf_map_get_info_by_fd(struct file *file,
				  struct bpf_map *map,
				  const union bpf_attr *attr,
				  union bpf_attr __user *uattr)
{
	struct bpf_map_info __user *uinfo = u64_to_user_ptr(attr->info.info);
	struct bpf_map_info info;
	u32 info_len = attr->info.info_len;
	int err;

	err = bpf_check_uarg_tail_zero(uinfo, sizeof(info), info_len);
	if (err)
		return err;
	info_len = min_t(u32, sizeof(info), info_len);

	memset(&info, 0, sizeof(info));
	info.type = map->map_type;
	info.id = map->id;
	info.key_size = map->key_size;
	info.value_size = map->value_size;
	info.max_entries = map->max_entries;
	info.map_flags = map->map_flags;
	memcpy(info.name, map->name, sizeof(map->name));

	if (map->btf) {
		info.btf_id = btf_obj_id(map->btf);
		info.btf_key_type_id = map->btf_key_type_id;
		info.btf_value_type_id = map->btf_value_type_id;
	}
	info.btf_vmlinux_value_type_id = map->btf_vmlinux_value_type_id;

	if (bpf_map_is_dev_bound(map)) {
		err = bpf_map_offload_info_fill(&info, map);
		if (err)
			return err;
	}

	if (copy_to_user(uinfo, &info, info_len) ||
	    put_user(info_len, &uattr->info.info_len))
		return -EFAULT;

	return 0;
}