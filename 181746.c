static void cmd_anal_opcode(RCore *core, const char *input) {
	int l, len = core->blocksize;
	ut32 tbs = core->blocksize;
	r_core_block_read (core);
	switch (input[0]) {
	case 's': // "aos"
	case 'j': // "aoj"
	case 'e': // "aoe"
	case 'r': {
		int count = 1;
		int obs = core->blocksize;
		if (input[1] && input[2]) {
			l = (int)r_num_get (core->num, input + 1);
			if (l > 0) {
				count = l;
			}
			l *= 8;
			if (l > obs) {
				r_core_block_size (core, l);
			}
		} else {
			count = 1;
		}
		core_anal_bytes (core, core->block, core->blocksize, count, input[0]);
		if (obs != core->blocksize) {
			r_core_block_size (core, obs);
		}
		}
		break;
	case 'm': // "aom"
		if (input[1] == '?') {
			r_cons_printf ("Usage: aom[ljd] [arg] .. list mnemonics for asm.arch\n");
			r_cons_printf (". = current, l = list, d = describe, j=json)\n");
		} else if (input[1] == 'd') {
			const int id = (input[2]==' ')
				?(int)r_num_math (core->num, input + 2): -1;
			char *ops = r_asm_mnemonics (core->assembler, id, false);
			if (ops) {
				char *ptr = ops;
				char *nl = strchr (ptr, '\n');
				while (nl) {
					*nl = 0;
					char *desc = r_asm_describe (core->assembler, ptr);
					if (desc) {
						const char *pad = r_str_pad (' ', 16 - strlen (ptr));
						r_cons_printf ("%s%s%s\n", ptr, pad, desc);
						free (desc);
					} else {
						r_cons_printf ("%s\n", ptr);
					}
					ptr = nl + 1;
					nl = strchr (ptr, '\n');
				}
				free (ops);
			}
		} else if (input[1] == 'l' || input[1] == '=' || input[1] == ' ' || input[1] == 'j') {
			if (input[1] == ' ' && !IS_DIGIT (input[2])) {
				r_cons_printf ("%d\n", r_asm_mnemonics_byname (core->assembler, input + 2));
			} else {
				const int id = (input[1] == ' ')
					?(int)r_num_math (core->num, input + 2): -1;
				char *ops = r_asm_mnemonics (core->assembler, id, input[1] == 'j');
				if (ops) {
					r_cons_println (ops);
					free (ops);
				}
			}
		} else {
			r_core_cmd0 (core, "ao~mnemonic[1]");
		}
		break;
	case 'c': // "aoc"
	{
		RList *hooks;
		RListIter *iter;
		RAnalCycleHook *hook;
		char *instr_tmp = NULL;
		int ccl = input[1]? r_num_math (core->num, &input[2]): 0; //get cycles to look for
		int cr = r_config_get_i (core->config, "asm.cmt.right");
		int fun = r_config_get_i (core->config, "asm.functions");
		int li = r_config_get_i (core->config, "asm.lines");
		int xr = r_config_get_i (core->config, "asm.xrefs");

		r_config_set_i (core->config, "asm.cmt.right", true);
		r_config_set_i (core->config, "asm.functions", false);
		r_config_set_i (core->config, "asm.lines", false);
		r_config_set_i (core->config, "asm.xrefs", false);

		hooks = r_core_anal_cycles (core, ccl); //analyse
		r_cons_clear_line (1);
		r_list_foreach (hooks, iter, hook) {
			instr_tmp = r_core_disassemble_instr (core, hook->addr, 1);
			r_cons_printf ("After %4i cycles:\t%s", (ccl - hook->cycles), instr_tmp);
			r_cons_flush ();
			free (instr_tmp);
		}
		r_list_free (hooks);

		r_config_set_i (core->config, "asm.cmt.right", cr); //reset settings
		r_config_set_i (core->config, "asm.functions", fun);
		r_config_set_i (core->config, "asm.lines", li);
		r_config_set_i (core->config, "asm.xrefs", xr);
	}
	break;
	case 'd': // "aod"
		if (input[1] == 'a') { // "aoda"
			// list sdb database
			sdb_foreach (core->assembler->pair, listOpDescriptions, core);
		} else if (input[1] == 0) {
			int cur = R_MAX (core->print->cur, 0);
			// XXX: we need cmd_xxx.h (cmd_anal.h)
			core_anal_bytes (core, core->block + cur, core->blocksize, 1, 'd');
		} else if (input[1] == ' ') {
			char *d = r_asm_describe (core->assembler, input + 2);
			if (d && *d) {
				r_cons_println (d);
				free (d);
			} else {
				eprintf ("Unknown mnemonic\n");
			}
		} else {
			eprintf ("Use: aod[?a] ([opcode])    describe current, [given] or all mnemonics\n");
		}
		break;
	case '*':
		r_core_anal_hint_list (core->anal, input[0]);
		break;
	case 0:
	case ' ': {
			int count = 0;
			if (input[0]) {
				l = (int)r_num_get (core->num, input + 1);
				if (l > 0) {
					count = l;
				}
				if (l > tbs) {
					r_core_block_size (core, l * 4);
					//len = l;
				}
			} else {
				len = l = core->blocksize;
				count = 1;
			}
			core_anal_bytes (core, core->block, len, count, 0);
		}
		break;
	default:
	case '?':
		r_core_cmd_help (core, help_msg_ao);
		break;
	}
}