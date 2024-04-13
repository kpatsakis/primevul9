static int compute_coverage(RCore *core) {
	RListIter *iter;
	SdbListIter *iter2;
	RAnalFunction *fcn;
	RIOMap *map;
	int cov = 0;
	cov += r_meta_get_size(core->anal, R_META_TYPE_DATA);
	r_list_foreach (core->anal->fcns, iter, fcn) {
		ls_foreach (core->io->maps, iter2, map) {
			if (map->perm & R_PERM_X) {
				ut64 section_end = map->itv.addr + map->itv.size;
				ut64 s = r_anal_function_realsize (fcn);
				if (fcn->addr >= map->itv.addr && (fcn->addr + s) < section_end) {
					cov += s;
				}
			}
		}
	}
	return cov;
}