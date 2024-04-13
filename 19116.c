static RList *recurse_bb(RCore *core, ut64 addr, RAnalBlock *dest) {
	RAnalBlock *bb = r_anal_bb_from_offset (core->anal, addr);
	if (bb == dest) {
		eprintf ("path found!");
		return NULL;
	}
	return recurse (core, bb, dest);
}