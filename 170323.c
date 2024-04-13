static int bpf_map_get_fd_by_id(const union bpf_attr *attr)
{
	struct bpf_map *map;
	u32 id = attr->map_id;
	int f_flags;
	int fd;

	if (CHECK_ATTR(BPF_MAP_GET_FD_BY_ID) ||
	    attr->open_flags & ~BPF_OBJ_FLAG_MASK)
		return -EINVAL;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	f_flags = bpf_get_file_flag(attr->open_flags);
	if (f_flags < 0)
		return f_flags;

	spin_lock_bh(&map_idr_lock);
	map = idr_find(&map_idr, id);
	if (map)
		map = __bpf_map_inc_not_zero(map, true);
	else
		map = ERR_PTR(-ENOENT);
	spin_unlock_bh(&map_idr_lock);

	if (IS_ERR(map))
		return PTR_ERR(map);

	fd = bpf_map_new_fd(map, f_flags);
	if (fd < 0)
		bpf_map_put_with_uref(map);

	return fd;
}