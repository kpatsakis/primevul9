static bool is_fcn_traced(RDebugTrace *traced, RAnalFunction *fcn) {
	int tag = traced->tag;
	RListIter *iter;
	RDebugTracepoint *trace;

	r_list_foreach (traced->traces, iter, trace) {
		if (!trace->tag || (tag & trace->tag)) {
			if (r_anal_function_contains (fcn, trace->addr)) {
				r_cons_printf ("\ntraced: %d\n", trace->times);
				return true;
			}
		}
	}
	return false;
}