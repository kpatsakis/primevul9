R_API int r_core_print_bb_gml(RCore *core, RAnalFunction *fcn) {
	RAnalBlock *bb;
	RListIter *iter;
	if (!fcn) {
		return false;
	}
	int id = 0;
	HtUUOptions opt = { 0 };
	HtUU *ht = ht_uu_new_opt (&opt);

	r_cons_printf ("graph\n[\n" "hierarchic 1\n" "label \"\"\n" "directed 1\n");

	r_list_foreach (fcn->bbs, iter, bb) {
		RFlagItem *flag = r_flag_get_i (core->flags, bb->addr);
		char *msg = flag? strdup (flag->name): r_str_newf ("0x%08"PFMT64x, bb->addr);
		// TODO char *str = r_str_escape_dot (msg);
#if USE_ID
		ht_uu_insert (ht, bb->addr, id);
		r_cons_printf ("  node [\n"
				"    id  %d\n"
				"    label  \"%s\"\n"
				"  ]\n", id, msg);
		id++;
#else
		r_cons_printf ("  node [\n"
				"    id  %"PFMT64d"\n"
				"    label  \"%s\"\n"
				"  ]\n", bb->addr, msg);
#endif
		free (msg);
	}

	r_list_foreach (fcn->bbs, iter, bb) {
		if (bb->addr == UT64_MAX) {
			continue;
		}

#if USE_ID
		if (bb->jump != UT64_MAX) {
			bool found;
			int i = ht_uu_find (ht, bb->addr, &found);
			if (found) {
				int i2 = ht_uu_find (ht, bb->jump, &found);
				if (found) {
					r_cons_printf ("  edge [\n"
							"    source  %d\n"
							"    target  %d\n"
							"  ]\n", i, i2);
				}
			}
		}
		if (bb->fail != UT64_MAX) {
			bool found;
			int i = ht_uu_find (ht, bb->addr, &found);
			if (found) {
				int i2 = ht_uu_find (ht, bb->fail, &found);
				if (found) {
					r_cons_printf ("  edge [\n"
						"    source  %d\n"
						"    target  %d\n"
						"  ]\n", i, i2);
				}
			}
		}
		if (bb->switch_op) {
			RListIter *it;
			RAnalCaseOp *cop;
			r_list_foreach (bb->switch_op->cases, it, cop) {
				bool found;
				int i = ht_uu_find (ht, bb->addr, &found);
				if (found) {
					int i2 = ht_uu_find (ht, cop->addr, &found);
					if (found) {
						r_cons_printf ("  edge [\n"
								"    source  %d\n"
								"    target  %d\n"
								"  ]\n", i, i2);
					}
				}
			}
		}
#else
		if (bb->jump != UT64_MAX) {
			r_cons_printf ("  edge [\n"
				"    source  %"PFMT64d"\n"
				"    target  %"PFMT64d"\n"
				"  ]\n", bb->addr, bb->jump
				);
		}
		if (bb->fail != UT64_MAX) {
			r_cons_printf ("  edge [\n"
				"    source  %"PFMT64d"\n"
				"    target  %"PFMT64d"\n"
				"  ]\n", bb->addr, bb->fail
				);
		}
		if (bb->switch_op) {
			RListIter *it;
			RAnalCaseOp *cop;
			r_list_foreach (bb->switch_op->cases, it, cop) {
				r_cons_printf ("  edge [\n"
					"    source  %"PFMT64d"\n"
					"    target  %"PFMT64d"\n"
					"  ]\n", bb->addr, cop->addr
					);
			}
		}
#endif
	}
	r_cons_printf ("]\n");
	ht_uu_free (ht);
	return true;
}