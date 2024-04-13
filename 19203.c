R_API int r_core_print_bb_custom(RCore *core, RAnalFunction *fcn) {
	RAnalBlock *bb;
	RListIter *iter;
	if (!fcn) {
		return false;
	}

	RConfigHold *hc = r_config_hold_new (core->config);
	r_config_hold (hc, "scr.color", "scr.utf8", "asm.marks", "asm.offset", "asm.lines",
	  "asm.cmt.right", "asm.cmt.col", "asm.lines.fcn", "asm.bytes", NULL);
	/*r_config_set_i (core->config, "scr.color", 0);*/
	r_config_set_i (core->config, "scr.utf8", 0);
	r_config_set_i (core->config, "asm.marks", 0);
	r_config_set_i (core->config, "asm.offset", 0);
	r_config_set_i (core->config, "asm.lines", 0);
	r_config_set_i (core->config, "asm.cmt.right", 0);
	r_config_set_i (core->config, "asm.cmt.col", 0);
	r_config_set_i (core->config, "asm.lines.fcn", 0);
	r_config_set_i (core->config, "asm.bytes", 0);

	r_list_foreach (fcn->bbs, iter, bb) {
		if (bb->addr == UT64_MAX) {
			continue;
		}
		char *title = get_title (bb->addr);
		char *body = r_core_cmd_strf (core, "pdb @ 0x%08"PFMT64x, bb->addr);
		char *body_b64 = r_base64_encode_dyn (body, -1);
		if (!title || !body || !body_b64) {
			free (body_b64);
			free (body);
			free (title);
			r_config_hold_restore (hc);
			r_config_hold_free (hc);
			return false;
		}
		body_b64 = r_str_prepend (body_b64, "base64:");
		r_cons_printf ("agn %s %s\n", title, body_b64);
		free (body);
		free (body_b64);
		free (title);
	}

	r_config_hold_restore (hc);
	r_config_hold_free (hc);

	r_list_foreach (fcn->bbs, iter, bb) {
		if (bb->addr == UT64_MAX) {
			continue;
		}
		char *u = get_title (bb->addr), *v = NULL;
		if (bb->jump != UT64_MAX) {
			v = get_title (bb->jump);
			r_cons_printf ("age %s %s\n", u, v);
			free (v);
		}
		if (bb->fail != UT64_MAX) {
			v = get_title (bb->fail);
			r_cons_printf ("age %s %s\n", u, v);
			free (v);
		}
		if (bb->switch_op) {
			RListIter *it;
			RAnalCaseOp *cop;
			r_list_foreach (bb->switch_op->cases, it, cop) {
				v = get_title (cop->addr);
				r_cons_printf ("age %s %s\n", u, v);
				free (v);
			}
		}
		free (u);
	}
	return true;
}