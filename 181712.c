static void cmd_anal_aad(RCore *core, const char *input) {
	RListIter *iter;
	RAnalRef *ref;
	RList *list = r_list_newf (NULL);
	r_anal_xrefs_from (core->anal, list, "xref", R_ANAL_REF_TYPE_DATA, UT64_MAX);
	r_list_foreach (list, iter, ref) {
		if (r_io_is_valid_offset (core->io, ref->addr, false)) {
			r_core_anal_fcn (core, ref->at, ref->addr, R_ANAL_REF_TYPE_NULL, 1);
		}
	}
	r_list_free (list);
}