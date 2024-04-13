static void update_uidmap(const char *path, int pid, char *map, size_t map_len)
{
	if (map == NULL || map_len <= 0)
		return;

	if (write_file(map, map_len, "/proc/%d/uid_map", pid) < 0) {
		if (errno != EPERM)
			bail("failed to update /proc/%d/uid_map", pid);
		if (try_mapping_tool(path, pid, map, map_len))
			bail("failed to use newuid map on %d", pid);
	}
}