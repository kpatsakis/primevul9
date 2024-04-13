static int core_anal_followptr(RCore *core, int type, ut64 at, ut64 ptr, ut64 ref, int code, int depth) {
	// SLOW Operation try to reduce as much as possible
	if (!ptr) {
		return false;
	}
	if (ref == UT64_MAX || ptr == ref) {
		const RAnalRefType t = code? type? type: R_ANAL_REF_TYPE_CODE: R_ANAL_REF_TYPE_DATA;
		r_anal_xrefs_set (core->anal, at, ptr, t);
		return true;
	}
	if (depth < 0) {
		return false;
	}
	int wordsize = (int)(core->anal->bits / 8);
	ut64 dataptr;
	if (!r_io_read_i (core->io, ptr, &dataptr, wordsize, false)) {
		// eprintf ("core_anal_followptr: Cannot read word at destination\n");
		return false;
	}
	return core_anal_followptr (core, type, at, dataptr, ref, code, depth - 1);
}