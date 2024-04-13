static int compute_code (RCore* core) {
	int code = 0;
	SdbListIter *iter;
	RIOMap *map;
	ls_foreach (core->io->maps, iter, map) {
		if (map->perm & R_PERM_X) {
			code += map->itv.size;
		}
	}
	return code;
}