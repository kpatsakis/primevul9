static fmode_t map_get_sys_perms(struct bpf_map *map, struct fd f)
{
	fmode_t mode = f.file->f_mode;

	/* Our file permissions may have been overridden by global
	 * map permissions facing syscall side.
	 */
	if (READ_ONCE(map->frozen))
		mode &= ~FMODE_CAN_WRITE;
	return mode;
}