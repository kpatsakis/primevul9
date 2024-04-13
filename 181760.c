static void cmd_anal_syscall(RCore *core, const char *input) {
	PJ *pj = NULL;
	RSyscallItem *si;
	RListIter *iter;
	RList *list;
	RNum *num = NULL;
	int n;

	switch (input[0]) {
	case 'c': // "asc"
		if (input[1] == 'a') {
			if (input[2] == ' ') {
				if (!isalpha ((ut8)input[3]) && (n = r_num_math (num, input + 3)) >= 0 ) {
					si = r_syscall_get (core->anal->syscall, n, -1);
					if (si) {
						r_cons_printf (".equ SYS_%s %s\n", si->name, syscallNumber (n));
					}
					else eprintf ("Unknown syscall number\n");
				} else {
					n = r_syscall_get_num (core->anal->syscall, input + 3);
					if (n != -1) {
						r_cons_printf (".equ SYS_%s %s\n", input + 3, syscallNumber (n));
					} else {
						eprintf ("Unknown syscall name\n");
					}
				}
			} else {
				list = r_syscall_list (core->anal->syscall);
				r_list_foreach (list, iter, si) {
					r_cons_printf (".equ SYS_%s %s\n",
						si->name, syscallNumber (si->num));
				}
				r_list_free (list);
			}
		} else {
			if (input[1] == ' ') {
				if (!isalpha ((ut8)input[2]) && (n = r_num_math (num, input + 2)) >= 0 ) {
					si = r_syscall_get (core->anal->syscall, n, -1);
					if (si) {
						r_cons_printf ("#define SYS_%s %s\n", si->name, syscallNumber (n));
					}
					else eprintf ("Unknown syscall number\n");
				} else {
					n = r_syscall_get_num (core->anal->syscall, input + 2);
					if (n != -1) {
						r_cons_printf ("#define SYS_%s %s\n", input + 2, syscallNumber (n));
					} else {
						eprintf ("Unknown syscall name\n");
					}
				}
			} else {
				list = r_syscall_list (core->anal->syscall);
				r_list_foreach (list, iter, si) {
					r_cons_printf ("#define SYS_%s %d\n",
						si->name, syscallNumber (si->num));
				}
				r_list_free (list);
			}
		}
		break;
	case 'f': // "asf"
		cmd_sdbk (core->anal->sdb_fcnsign, input + 1);
		break;
	case 'k': // "ask"
		cmd_sdbk (core->anal->syscall->db, input + 1);
		break;
	case 'l': // "asl"
		if (input[1] == ' ') {
			if (!isalpha ((ut8)input[2]) && (n = r_num_math (num, input + 2)) >= 0 ) {
				si = r_syscall_get (core->anal->syscall, n, -1);
				if (si)
					r_cons_println (si->name);
				else eprintf ("Unknown syscall number\n");
			} else {
				n = r_syscall_get_num (core->anal->syscall, input + 2);
				if (n != -1) {
					r_cons_printf ("%s\n", syscallNumber (n));
				} else {
					eprintf ("Unknown syscall name\n");
				}
			}
		} else {
			list = r_syscall_list (core->anal->syscall);
			r_list_foreach (list, iter, si) {
				r_cons_printf ("%s = 0x%02x.%s\n",
					si->name, si->swi, syscallNumber (si->num));
			}
			r_list_free (list);
		}
		break;
	case 'j': // "asj"
		pj = pj_new ();
		pj_a (pj);
		list = r_syscall_list (core->anal->syscall);
		r_list_foreach (list, iter, si) {
			pj_o (pj);
			pj_ks (pj, "name", si->name);
			pj_ki (pj, "swi", si->swi);
			pj_ki (pj, "num", si->num);
			pj_end (pj);
		}
		pj_end (pj);
		if (pj) {
			r_cons_println (pj_string (pj));
			pj_free (pj);
		}
		break;
	case '\0':
		cmd_syscall_do (core, -1, core->offset);
		break;
	case ' ':
		{
		const char *sn = r_str_trim_head_ro (input + 1);
		st64 num = r_syscall_get_num (core->anal->syscall, sn);
		if (num < 1) {
			num = (int)r_num_get (core->num, sn);
		}
		cmd_syscall_do (core, num, -1);
		}
		break;
	default:
	case '?':
		r_core_cmd_help (core, help_msg_as);
		break;
	}
}