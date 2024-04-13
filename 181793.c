static int var_cmd(RCore *core, const char *str) {
	int delta, type = *str, res = true;
	RAnalVar *v1;
	if (!str[0]) {
		// "afv"
		r_core_cmd0 (core, "afvs");
		r_core_cmd0 (core, "afvb");
		r_core_cmd0 (core, "afvr");
		return true;
	}
	if (!str[0] || str[1] == '?'|| str[0] == '?') {
		var_help (core, *str);
		return res;
	}
	if (str[0] == 'j') {
		// "afvj"
		r_cons_printf ("{\"sp\":");
		r_core_cmd0 (core, "afvsj");
		r_cons_printf (",\"bp\":");
		r_core_cmd0 (core, "afvbj");
		r_cons_printf (",\"reg\":");
		r_core_cmd0 (core, "afvrj");
		r_cons_printf ("}\n");
		return true;
	}
	char *p = strdup (str);
	char *ostr = p;
	RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, -1);
	/* Variable access CFvs = set fun var */
	switch (str[0]) {
	case '-':
		// "afv"
		r_core_cmdf (core, "afvs-%s", str + 1);
		r_core_cmdf (core, "afvb-%s", str + 1);
		r_core_cmdf (core, "afvr-%s", str + 1);
		return true;
	case 'R': // "afvR"
	case 'W': // "afvW"
	case '*': // "afv*"
		if (fcn) {
			const char *name = strchr (ostr, ' ');
			if (name) {
				name = r_str_trim_head_ro (name);
			}
			list_vars (core, fcn, str[0], name);
			return true;
		} else {
			eprintf ("afv: Cannot find function in 0x%08"PFMT64x"\n", core->offset);
			return false;
		}
	case 'a': // "afva"
		if (fcn) {
			r_anal_var_delete_all (core->anal, fcn->addr, R_ANAL_VAR_KIND_REG);
			r_anal_var_delete_all (core->anal, fcn->addr, R_ANAL_VAR_KIND_BPV);
			r_anal_var_delete_all (core->anal, fcn->addr, R_ANAL_VAR_KIND_SPV);
			r_core_recover_vars (core, fcn, false);
			free (p);
			return true;
		} else {
			eprintf ("afv: Cannot find function in 0x%08"PFMT64x"\n", core->offset);
			return false;
		}
	case 'n':
		if (str[1]) { // "afvn"
			RAnalOp *op = r_core_anal_op (core, core->offset, R_ANAL_OP_MASK_BASIC);
			const char *new_name = r_str_trim_head_ro (strchr (ostr, ' '));
			if (!new_name) {
				r_anal_op_free (op);
				free (ostr);
				return false;
			}
			char *old_name = strchr (new_name, ' ');
			if (!old_name) {
				if (op && op->var) {
					old_name = op->var->name;
				} else {
					eprintf ("Cannot find var @ 0x%08"PFMT64x"\n", core->offset);
					r_anal_op_free (op);
					free (ostr);
					return false;
				}
			} else {
				*old_name++ = 0;
				r_str_trim (old_name);
			}
			if (fcn) {
				v1 = r_anal_var_get_byname (core->anal, fcn->addr, old_name);
				if (v1) {
					r_anal_var_rename (core->anal, fcn->addr, R_ANAL_VAR_SCOPE_LOCAL,
							v1->kind, old_name, new_name, true);
					r_anal_var_free (v1);
				} else {
					eprintf ("Cant find var by name\n");
				}
			} else {
				eprintf ("afv: Cannot find function in 0x%08"PFMT64x"\n", core->offset);
				r_anal_op_free (op);
				free (ostr);
				return false;
			}
			r_anal_op_free (op);
			free (ostr);
		} else {
			RListIter *iter;
			RAnalVar *v;
			RList *list = r_anal_var_all_list (core->anal, fcn);
			r_list_foreach (list, iter, v) {
				r_cons_printf ("%s\n", v->name);
			}
		}
		return true;
	case 'd': // "afvd"
		if (!fcn) {
			eprintf ("Cannot find function.\n");
		} else if (str[1]) {
			p = strchr (ostr, ' ');
			if (!p) {
				free (ostr);
				return false;
			}
			r_str_trim (p);
			v1 = r_anal_var_get_byname (core->anal, fcn->addr, p);
			if (!v1) {
				free (ostr);
				return false;
			}
			r_anal_var_display (core->anal, v1->delta, v1->kind, v1->type);
			r_anal_var_free (v1);
			free (ostr);
		} else {
			RListIter *iter;
			RAnalVar *p;
			RList *list = r_anal_var_all_list (core->anal, fcn);
			r_list_foreach (list, iter, p) {
				char *a = r_core_cmd_strf (core, ".afvd %s", p->name);
				if ((a && !*a) || !a) {
					free (a);
					a = strdup ("\n");
				}
				r_cons_printf ("%s %s = %s", p->isarg ? "arg": "var", p->name, a);
				free (a);
			}
			r_list_free (list);
		}
		return true;
	case 'f': // "afvf"
		__cmd_afvf (core, ostr);
		break;
	case 't':
		if (fcn) { // "afvt"
			p = strchr (ostr, ' ');
			if (!p++) {
				free (ostr);
				return false;
			}

			char *type = strchr (p, ' ');
			if (!type) {
				free (ostr);
				return false;
			}
			*type++ = 0;
			v1 = r_anal_var_get_byname (core->anal, fcn->addr, p);
			if (!v1) {
				eprintf ("Cant find get by name %s\n", p);
				free (ostr);
				return false;
			}
			r_anal_var_retype (core->anal, fcn->addr,
					R_ANAL_VAR_SCOPE_LOCAL, -1, v1->kind, type, -1, v1->isarg, p);
			r_anal_var_free (v1);
			free (ostr);
			return true;
		} else {
			eprintf ("Cannot find function\n");
			return false;
		}
	}
	switch (str[1]) { // afv[bsr]
	case '\0':
	case '*':
		r_anal_var_list_show (core->anal, fcn, type, str[1], NULL);
		break;
	case 'j': {
		PJ *pj = pj_new ();
		if (!pj) {
			return -1;
		}
		r_anal_var_list_show (core->anal, fcn, type, str[1], pj);
		r_cons_println (pj_string (pj));
		pj_free (pj);
	}
		break;
	case '.':
		r_anal_var_list_show (core->anal, fcn, core->offset, 0, NULL);
		break;
	case '-': // "afv[bsr]-"
		if (!fcn) {
			eprintf ("Cannot find function\n");
			return false;
		}
		if (str[2] == '*') {
			r_anal_var_delete_all (core->anal, fcn->addr, type);
		} else {
			if (IS_DIGIT (str[2])) {
				r_anal_var_delete (core->anal, fcn->addr,
						type, 1, (int)r_num_math (core->num, str + 1));
			} else {
				char *name = r_str_trim_dup (str + 2);
				if (name) {
					r_anal_var_delete_byname (core->anal, fcn, type, name);
					free (name);
				}
			}
		}
		break;
	case 'd':
		eprintf ("This command is deprecated, use afvd instead\n");
		break;
	case 't':
		eprintf ("This command is deprecated use afvt instead\n");
		break;
	case 's':
	case 'g':
		if (str[2] != '\0') {
			int rw = 0; // 0 = read, 1 = write
			int idx = r_num_math (core->num, str + 2);
			char *vaddr;
			char *p = strchr (ostr, ' ');
			if (!p) {
				var_help (core, type);
				break;
			}
			r_str_trim (p);
			ut64 addr = core->offset;
			if ((vaddr = strchr (p , ' '))) {
				addr = r_num_math (core->num, vaddr);
			}
			RAnalVar *var = r_anal_var_get (core->anal, fcn->addr,
							str[0], R_ANAL_VAR_SCOPE_LOCAL, idx);
			if (!var) {
				eprintf ("Cannot find variable with delta %d\n", idx);
				res = false;
				break;
			}
			rw = (str[1] == 'g')? 0: 1;
			int ptr = *var->type == 's' ? idx - fcn->maxstack : idx;
			r_anal_var_access (core->anal, fcn->addr, str[0],
					R_ANAL_VAR_SCOPE_LOCAL, idx, ptr, rw, addr);
			r_anal_var_free (var);
		} else {
			eprintf ("Missing argument\n");
		}
		break;
	case ' ': {
		const char *name;
		char *vartype;
		bool isarg = false;
		int size = 4;
		int scope = 1;
		for (str++; *str == ' ';) str++;
		p = strchr (str, ' ');
		if (!p) {
			var_help (core, type);
			break;
		}
		*p++ = 0;
		if (type == 'r') { //registers
			RRegItem *i = r_reg_get (core->anal->reg, str, -1);
			if (!i) {
				eprintf ("Register not found");
				break;
			}
			delta = i->index;
			isarg = true;
		} else {
			delta = r_num_math (core->num, str);
		}
		name = p;
		if (!name) {
			eprintf ("Missing name\n");
			break;
		}
		vartype = strchr (name, ' ');
		if (!vartype) {
			vartype = "int";
		} else {
			*vartype++ = 0;
		}
		if ((type == 'b') && delta > 0) {
			isarg = true;
		} else if (type == 's' && fcn && delta > fcn->maxstack) {
			isarg = true;
		}
		if (fcn) {
			r_anal_var_add (core->anal, fcn->addr,scope,
					delta, type, vartype,
					size, isarg, name);
		} else {
			eprintf ("Missing function at 0x%08"PFMT64x"\n", core->offset);
		}
 		}
		break;
	}
	free (ostr);
	return res;
}