static void anal_axg (RCore *core, const char *input, int level, Sdb *db, int opts, PJ* pj) {
	char arg[32], pre[128];
	RListIter *iter;
	RAnalRef *ref;
	ut64 addr = core->offset;
	bool is_json = opts & R_CORE_ANAL_JSON;
	bool is_r2 = opts & R_CORE_ANAL_GRAPHBODY;
	if (is_json && !pj) {
		return;
	}
	if (input && *input) {
		addr = r_num_math (core->num, input);
	}
	// eprintf ("Path between 0x%08"PFMT64x" .. 0x%08"PFMT64x"\n", core->offset, addr);
	int spaces = (level + 1) * 2;
	if (spaces > sizeof (pre) - 4) {
		spaces = sizeof (pre) - 4;
	}
	memset (pre, ' ', sizeof (pre));
	strcpy (pre + spaces, "- ");

	RList *xrefs = r_anal_xrefs_get (core->anal, addr);
	bool open_object = false;
	if (!r_list_empty (xrefs)) {
		RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, addr, -1);
		if (fcn) {
			if (is_r2) {
				r_cons_printf ("agn 0x%08"PFMT64x" %s\n", fcn->addr, fcn->name);
			} else if (is_json) {
				char taddr[64];
				pj_o (pj);
				pj_k (pj, sdb_itoa (addr, taddr, 10));
				pj_o (pj);
				pj_ks (pj, "type", "fcn");
				pj_kn (pj, "fcn_addr", fcn->addr);
				pj_ks (pj, "name", fcn->name);
				pj_k (pj, "refs");
				pj_a (pj);
				open_object = true;
			} else {
				//if (sdb_add (db, fcn->name, "1", 0)) {
				r_cons_printf ("%s0x%08"PFMT64x" fcn 0x%08"PFMT64x" %s\n",
					pre + 2, addr, fcn->addr, fcn->name);
				//}
			}
		} else {
			if (is_r2) {
				r_cons_printf ("age 0x%08"PFMT64x"\n", addr);
			} else if (is_json) {
				char taddr[64];
				pj_o (pj);
				pj_k (pj, sdb_itoa (addr, taddr, 10));
				pj_o (pj);
				pj_k (pj, "refs");
				pj_a (pj);
				open_object = true;
			} else {
			//snprintf (arg, sizeof (arg), "0x%08"PFMT64x, addr);
			//if (sdb_add (db, arg, "1", 0)) {
				r_cons_printf ("%s0x%08"PFMT64x"\n", pre+2, addr);
			//}
			}
		}
	}
	r_list_foreach (xrefs, iter, ref) {
		RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, ref->addr, -1);
		if (fcn) {
			if (is_r2) {
				r_cons_printf ("agn 0x%08"PFMT64x" %s\n", fcn->addr, fcn->name);
				r_cons_printf ("age 0x%08"PFMT64x" 0x%08"PFMT64x"\n", fcn->addr, addr);
			} else if (is_json) {
				if (level == 0) {
					char taddr[64];
					pj_o (pj);
					pj_k (pj, sdb_itoa (ref->addr, taddr, 10));
					pj_o (pj);
					pj_ks (pj, "type", "fcn");
					pj_kn (pj, "fcn_addr", fcn->addr);
					pj_ks (pj, "name", fcn->name);
					pj_k (pj, "refs");
					pj_a (pj);
					open_object = true;
				} else {
					char taddr[64];
					pj_end (pj);
					pj_end (pj);
					pj_end (pj);
					pj_o (pj);
					pj_k (pj, sdb_itoa (ref->addr, taddr, 10));
					pj_o (pj);
					pj_ks (pj, "type", "fcn");
					pj_kn (pj, "fcn_addr", fcn->addr);
					pj_ks (pj, "refs", fcn->name);
					pj_k (pj, "refs");
					pj_a (pj);

				}
			} else {
				r_cons_printf ("%s0x%08"PFMT64x" fcn 0x%08"PFMT64x" %s\n", pre, ref->addr, fcn->addr, fcn->name);
			}
			if (sdb_add (db, fcn->name, "1", 0)) {
				snprintf (arg, sizeof (arg), "0x%08"PFMT64x, fcn->addr);
				anal_axg (core, arg, level + 1, db, opts, pj);
			} else {
				if (is_json) {
					pj_end (pj);
					pj_end (pj);
					pj_end (pj);
					open_object = false;
				}
			}
		} else {
			if (is_r2) {
				r_cons_printf ("agn 0x%08"PFMT64x" ???\n", ref->addr);
				r_cons_printf ("age 0x%08"PFMT64x" 0x%08"PFMT64x"\n", ref->addr, addr);
			} else if (is_json) {
				char taddr[64];
				pj_o (pj);
				pj_k (pj, sdb_itoa (ref->addr, taddr, 10));
				pj_o (pj);
				pj_ks (pj, "type", "???");
				pj_k (pj, "refs");
				pj_a (pj);
				open_object = true;
			} else {
				r_cons_printf ("%s0x%08"PFMT64x" ???\n", pre, ref->addr);
			}
			snprintf (arg, sizeof (arg), "0x%08"PFMT64x, ref->addr);
			if (sdb_add (db, arg, "1", 0)) {
				anal_axg (core, arg, level + 1, db, opts, pj);
			} else {
				if (is_json) {
					pj_end (pj);
					pj_end (pj);
					pj_end (pj);
					open_object = false;
				}
			}
		}
	}
	if (is_json) {
		if (open_object) {
			pj_end (pj);
			pj_end (pj);
			pj_end (pj);
		}
		if (level == 0) {
			if (open_object) {
				pj_end (pj);
				pj_end (pj);
				pj_end (pj);
			}
		}
	}
	r_list_free (xrefs);
}