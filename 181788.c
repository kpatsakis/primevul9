void cmd_anal_reg(RCore *core, const char *str) {
	if (0) {
		/* enable this block when dr and ar use the same code but just using
		   core->dbg->reg or core->anal->reg and removing all the debugger
		   dependent code */
		RReg *reg = core->dbg->reg;
		core->dbg->reg = core->anal->reg;
		cmd_debug_reg (core, str);
		core->dbg->reg = reg;
		return;
	}

	int size = 0, i, type = R_REG_TYPE_GPR;
	int bits = (core->anal->bits & R_SYS_BITS_64)? 64: 32;
	int use_colors = r_config_get_i (core->config, "scr.color");
	RRegItem *r;
	const char *use_color;
	const char *name;
	char *arg;

	if (use_colors) {
#define ConsP(x) (core->cons && core->cons->context->pal.x)? core->cons->context->pal.x
		use_color = ConsP (creg)
		: Color_BWHITE;
	} else {
		use_color = NULL;
	}
	switch (str[0]) {
	case 'l': // "arl"
	{
		const bool use_json = str[1] == 'j';
		RRegSet *rs = r_reg_regset_get (core->anal->reg, R_REG_TYPE_GPR);
		if (rs) {
			RRegItem *r;
			RListIter *iter;
			PJ *pj = pj_new ();
			pj_a (pj);
			r_list_foreach (rs->regs, iter, r) {
				if (use_json) {
					pj_s (pj, r->name);
				} else {
					r_cons_println (r->name);
				}
			}
			if (use_json) {
				pj_end (pj);
				const char *s = pj_string (pj);
				r_cons_println (s);
			}
			pj_free (pj);
		}
	} break;
	case ',': // "ar,"
		__tableRegList (core, core->anal->reg, str + 1);
		break;
	case '0': // "ar0"
		r_reg_arena_zero (core->anal->reg);
		break;
	case 'A': // "arA"
		{
			int nargs = 4;
			RReg *reg = core->anal->reg;
			for (i = 0; i < nargs; i++) {
				const char *name = r_reg_get_name (reg, r_reg_get_name_idx (sdb_fmt ("A%d", i)));
				ut64 off = r_reg_getv (core->anal->reg, name);
				r_cons_printf ("0x%08"PFMT64x" ", off);
				// XXX very ugly hack
				char *s = r_core_cmd_strf (core, "pxr 32 @ 0x%08"PFMT64x, off);
				if (s) {
					char *nl = strchr (s, '\n');
					if (nl) {
						*nl = 0;
						r_cons_printf ("%s\n", s);
					}
					free (s);
				}
//				r_core_cmd0 (core, "ar A0,A1,A2,A3");
			}
		}
		break;
	case 'C': // "arC"
		if (core->anal->reg->reg_profile_cmt) {
			r_cons_println (core->anal->reg->reg_profile_cmt);
		}
		break;
	case 'w': // "arw"
		switch (str[1]) {
		case '?': {
			r_core_cmd_help (core, help_msg_arw);
			break;
		}
		case ' ':
			r_reg_arena_set_bytes (core->anal->reg, str + 1);
			break;
		default:
			r_core_cmd_help (core, help_msg_arw);
			break;
		}
		break;
	case 'a': // "ara"
		switch (str[1]) {
		case '?': // "ara?"
			r_core_cmd_help (core, help_msg_ara);
			break;
		case 's': // "aras"
			r_reg_arena_swap (core->anal->reg, false);
			break;
		case '+': // "ara+"
			r_reg_arena_push (core->anal->reg);
			break;
		case '-': // "ara-"
			r_reg_arena_pop (core->anal->reg);
			break;
		default: {
			int i, j;
			RRegArena *a;
			RListIter *iter;
			for (i = 0; i < R_REG_TYPE_LAST; i++) {
				RRegSet *rs = &core->anal->reg->regset[i];
				j = 0;
				r_list_foreach (rs->pool, iter, a) {
					r_cons_printf ("%s %p %d %d %s %d\n",
						(a == rs->arena)? "*": ".", a,
						i, j, r_reg_get_type (i), a->size);
					j++;
				}
			}
		} break;
		}
		break;
	case '?': // "ar?"
		if (str[1]) {
			ut64 off = r_reg_getv (core->anal->reg, str + 1);
			r_cons_printf ("0x%08" PFMT64x "\n", off);
		} else {
			r_core_cmd_help (core, help_msg_ar);
		}
		break;
	case 'r': // "arr"
		switch (str[1]) {
		case 'j': // "arrj"
			r_core_debug_rr (core, core->anal->reg, 'j');
			break;
		default:
			r_core_debug_rr (core, core->anal->reg, 0);
			break;
		}
		break;
	case 'S': { // "arS"
		int sz;
		ut8 *buf = r_reg_get_bytes (core->anal->reg, R_REG_TYPE_GPR, &sz);
		r_cons_printf ("%d\n", sz);
		free (buf);
		} break;
	case 'b': { // "arb" WORK IN PROGRESS // DEBUG COMMAND
		int len, type = R_REG_TYPE_GPR;
		arg = strchr (str, ' ');
		if (arg) {
			char *string = r_str_trim_dup (arg + 1);
			if (string) {
				type = r_reg_type_by_name (string);
				if (type == -1 && string[0] != 'a') {
					type = R_REG_TYPE_GPR;
				}
				free (string);
			}
		}
		ut8 *buf = r_reg_get_bytes (core->dbg->reg, type, &len);
		if (buf) {
			r_print_hexdump (core->print, 0LL, buf, len, 32, 4, 1);
			free (buf);
		}
		} break;
	case 'c': // "arc"
		// TODO: set flag values with drc zf=1
		{
			RRegItem *r;
			const char *name = r_str_trim_head_ro (str + 1);
			if (*name && name[1]) {
				r = r_reg_cond_get (core->dbg->reg, name);
				if (r) {
					r_cons_println (r->name);
				} else {
					int id = r_reg_cond_from_string (name);
					RRegFlags *rf = r_reg_cond_retrieve (core->dbg->reg, NULL);
					if (rf) {
						int o = r_reg_cond_bits (core->dbg->reg, id, rf);
						core->num->value = o;
						// ORLY?
						r_cons_printf ("%d\n", o);
						free (rf);
					} else {
						eprintf ("unknown conditional or flag register\n");
					}
				}
			} else {
				RRegFlags *rf = r_reg_cond_retrieve (core->dbg->reg, NULL);
				if (rf) {
					r_cons_printf ("| s:%d z:%d c:%d o:%d p:%d\n",
						rf->s, rf->z, rf->c, rf->o, rf->p);
					if (*name == '=') {
						for (i = 0; i < R_REG_COND_LAST; i++) {
							r_cons_printf ("%s:%d ",
								r_reg_cond_to_string (i),
								r_reg_cond_bits (core->dbg->reg, i, rf));
						}
						r_cons_newline ();
					} else {
						for (i = 0; i < R_REG_COND_LAST; i++) {
							r_cons_printf ("%d %s\n",
								r_reg_cond_bits (core->dbg->reg, i, rf),
								r_reg_cond_to_string (i));
						}
					}
					free (rf);
				}
			}
		}
		break;
	case 's': // "ars"
		switch (str[1]) {
		case '-': // "ars-"
			r_reg_arena_pop (core->dbg->reg);
			// restore debug registers if in debugger mode
			r_debug_reg_sync (core->dbg, R_REG_TYPE_GPR, true);
			break;
		case '+': // "ars+"
			r_reg_arena_push (core->dbg->reg);
			break;
		case '?': { // "ars?"
			// TODO #7967 help refactor: dup from drp
			const char *help_msg[] = {
				"Usage:", "drs", " # Register states commands",
				"drs", "", "List register stack",
				"drs+", "", "Push register state",
				"drs-", "", "Pop register state",
				NULL };
			r_core_cmd_help (core, help_msg);
		} break;
		default:
			r_cons_printf ("%d\n", r_list_length (
						core->dbg->reg->regset[0].pool));
			break;
		}
		break;
	case 'p': // "arp"
		// XXX we have to break out .h for these cmd_xxx files.
		cmd_reg_profile (core, 'a', str);
		break;
	case 't': // "art"
		for (i = 0; (name = r_reg_get_type (i)); i++) {
			r_cons_println (name);
		}
		break;
	case 'n': // "arn"
		if (*(str + 1) == '\0') {
			eprintf ("Oops. try arn [PC|SP|BP|A0|A1|A2|A3|A4|R0|R1|ZF|SF|NF|OF]\n");
			break;
		}
		name = r_reg_get_name (core->dbg->reg, r_reg_get_name_idx (str + 2));
		if (name && *name) {
			r_cons_println (name);
		} else {
			eprintf ("Oops. try arn [PC|SP|BP|A0|A1|A2|A3|A4|R0|R1|ZF|SF|NF|OF]\n");
		}
		break;
	case 'd': // "ard"
		r_debug_reg_list (core->dbg, R_REG_TYPE_GPR, bits, 3, use_color); // XXX detect which one is current usage
		break;
	case 'o': // "aro"
		r_reg_arena_swap (core->dbg->reg, false);
		r_debug_reg_list (core->dbg, R_REG_TYPE_GPR, bits, 0, use_color); // XXX detect which one is current usage
		r_reg_arena_swap (core->dbg->reg, false);
		break;
	case '=': // "ar="
		{
			char *p = NULL;
			char *bits = NULL;
			if (str[1]) {
				p = strdup (str + 1);
				if (str[1] != ':') {
					// Bits were specified
					bits = strtok (p, ":");
					if (r_str_isnumber (bits)) {
						st64 sz = r_num_math (core->num, bits);
						if (sz > 0) {
							size = sz;
						}
					} else {
						r_core_cmd_help (core, help_msg_ar);
						break;
					}
				}
				int len = bits ? strlen (bits) : 0;
				if (str[len + 1] == ':') {
					// We have some regs
					char *regs = bits ? strtok (NULL, ":") : strtok ((char *)str + 1, ":");
					char *reg = strtok (regs, " ");
					RList *q_regs = r_list_new ();
					if (q_regs) {
						while (reg) {
							r_list_append (q_regs, reg);
							reg = strtok (NULL, " ");
						}
						core->dbg->q_regs = q_regs;
					}
				}
			}
			__anal_reg_list (core, type, size, str[0]);
			if (!r_list_empty (core->dbg->q_regs)) {
				r_list_free (core->dbg->q_regs);
			}
			core->dbg->q_regs = NULL;
			free (p);
		}
		break;
	case '.': // "ar."
	case '-': // "ar-"
	case '*': // "ar*"
	case 'R': // "arR"
	case 'j': // "arj"
	case 'i': // "arj"
	case '\0': // "ar"
		__anal_reg_list (core, type, size, str[0]);
		break;
	case ' ': { // "ar "
		arg = strchr (str + 1, '=');
		if (arg) {
			*arg = 0;
			char *ostr = r_str_trim_dup (str + 1);
			char *regname = r_str_trim_nc (ostr);
			r = r_reg_get (core->dbg->reg, regname, -1);
			if (!r) {
				int role = r_reg_get_name_idx (regname);
				if (role != -1) {
					const char *alias = r_reg_get_name (core->dbg->reg, role);
					r = r_reg_get (core->dbg->reg, alias, -1);
				}
			}
			if (r) {
				//eprintf ("%s 0x%08"PFMT64x" -> ", str,
				//	r_reg_get_value (core->dbg->reg, r));
				r_reg_set_value (core->dbg->reg, r,
						r_num_math (core->num, arg + 1));
				r_debug_reg_sync (core->dbg, R_REG_TYPE_ALL, true);
				//eprintf ("0x%08"PFMT64x"\n",
				//	r_reg_get_value (core->dbg->reg, r));
				r_core_cmdf (core, ".dr*%d", bits);
			} else {
				eprintf ("ar: Unknown register '%s'\n", regname);
			}
			free (ostr);
			return;
		}
		char name[32];
		int i = 1, j;
		while (str[i]) {
			if (str[i] == ',') {
				i++;
			} else {
				for (j = i; str[++j] && str[j] != ','; );
				if (j - i + 1 <= sizeof name) {
					r_str_ncpy (name, str + i, j - i + 1);
					if (IS_DIGIT (name[0])) { // e.g. ar 32
						__anal_reg_list (core, R_REG_TYPE_GPR, atoi (name), '\0');
					} else if (showreg (core, name) > 0) { // e.g. ar rax
					} else { // e.g. ar gpr ; ar all
						type = r_reg_type_by_name (name);
						// TODO differentiate ALL and illegal register types and print error message for the latter
						__anal_reg_list (core, type, -1, '\0');
					}
				}
				i = j;
			}
		}
	}
	}
}