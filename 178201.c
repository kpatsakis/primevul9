static void update_gidmap(const char *path, int pid, char *map, size_t map_len)
{
	if (map == NULL || map_len <= 0)
		return;

	if (write_file(map, map_len, "/proc/%d/gid_map", pid) < 0) {
		if (errno != EPERM)
			bail("failed to update /proc/%d/gid_map", pid);
		if (try_mapping_tool(path, pid, map, map_len))
			bail("failed to use newgid map on %d", pid);
	}
}