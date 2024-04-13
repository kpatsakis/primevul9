static int cmd_an(RCore *core, bool use_json, const char *name)
{
	ut64 off = core->offset;
	RAnalOp op;
	char *q = NULL;
	PJ *pj = NULL;
	ut64 tgt_addr = UT64_MAX;

	if (use_json) {
		pj = pj_new ();
		pj_a (pj);
	}

	r_anal_op (core->anal, &op, off,
			core->block + off - core->offset, 32, R_ANAL_OP_MASK_BASIC);

	tgt_addr = op.jump != UT64_MAX ? op.jump : op.ptr;
	if (op.var) {
		RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, off, 0);
		if (fcn) {
			RAnalVar *bar = r_anal_var_get_byname (core->anal, fcn->addr, op.var->name);
			if (bar) {
				if (name) {
					r_anal_var_rename (core->anal, fcn->addr, bar->scope,
									bar->kind, bar->name, name, true);
				} else if (!use_json) {
					r_cons_println (bar->name);
				} else {
					pj_o (pj);
					pj_ks (pj, "name", bar->name);
					pj_ks (pj, "type", "var");
					pj_kn (pj, "offset", tgt_addr);
					pj_end (pj);
				}
			} else {
				eprintf ("Cannot find variable\n");
			}
		} else {
			eprintf ("Cannot find function\n");
		}
	} else if (tgt_addr != UT64_MAX) {
		RAnalFunction *fcn = r_anal_get_function_at (core->anal, tgt_addr);
		RFlagItem *f = r_flag_get_i (core->flags, tgt_addr);
		if (fcn) {
			if (name) {
				q = r_str_newf ("afn %s 0x%"PFMT64x, name, tgt_addr);
			} else if (!use_json) {
				r_cons_println (fcn->name);
			} else {
				pj_o (pj);
				pj_ks (pj, "name", fcn->name);
				pj_ks (pj, "type", "function");
				pj_kn (pj, "offset", tgt_addr);
				pj_end (pj);
			}
		} else if (f) {
			if (name) {
				q = r_str_newf ("fr %s %s", f->name, name);
			} else if (!use_json) {
				r_cons_println (f->name);
			} else {
				pj_o (pj);
				if (name) {
					pj_ks (pj, "old_name", f->name);
					pj_ks (pj, "new_name", name);
				} else {
					pj_ks (pj, "name", f->name);
				}
				if (f->realname) {
					pj_ks (pj, "realname", f->realname);
				}
				pj_ks (pj, "type", "flag");
				pj_kn (pj, "offset", tgt_addr);
				pj_end (pj);
			}
		} else {
			if (name) {
				q = r_str_newf ("f %s @ 0x%"PFMT64x, name, tgt_addr);
			} else if (!use_json) {
				r_cons_printf ("0x%" PFMT64x "\n", tgt_addr);
			} else {
				pj_o (pj);
				pj_ks (pj, "name", name);
				pj_ks (pj, "type", "address");
				pj_kn (pj, "offset", tgt_addr);
				pj_end (pj);
			}
		}
	}

	if (use_json) {
		pj_end (pj);
	}

	if (pj) {
		r_cons_println (pj_string (pj));
		pj_free (pj);
	}

	if (q) {
		r_core_cmd0 (core, q);
		free (q);
	}
	r_anal_op_fini (&op);
	return 0;
}