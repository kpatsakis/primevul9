static void loganal(ut64 from, ut64 to, int depth) {
	r_cons_clear_line (1);
	eprintf ("0x%08"PFMT64x" > 0x%08"PFMT64x" %d\r", from, to, depth);
}