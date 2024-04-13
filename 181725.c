static void cmd_anal_abt(RCore *core, const char *input) {
	switch (*input) {
	case 'e':
		{
		int n = 1;
		char *p = strchr (input + 1, ' ');
		if (!p) {
			eprintf ("Usage: abte [addr] # emulate from beginning of function to the given address.\n");
			return;
		}
		ut64 addr = r_num_math (core->num, p + 1);
		RList *paths = r_core_anal_graph_to (core, addr, n);
		if (paths) {
			RAnalBlock *bb;
			RList *path;
			RListIter *pathi;
			RListIter *bbi;
			r_cons_printf ("f orip=`dr?PC`\n");
			r_list_foreach (paths, pathi, path) {
				r_list_foreach (path, bbi, bb) {
					r_cons_printf ("# 0x%08" PFMT64x "\n", bb->addr);
					if (addr >= bb->addr && addr < bb->addr + bb->size) {
						r_cons_printf ("aepc 0x%08"PFMT64x"\n", bb->addr);
						r_cons_printf ("aesou 0x%08"PFMT64x"\n", addr);
					} else {
						r_cons_printf ("aepc 0x%08"PFMT64x"\n", bb->addr);
						r_cons_printf ("aesou 0x%08"PFMT64x"\n", bb->addr + bb->size);
					}
				}
				r_cons_newline ();
				r_list_purge (path);
				free (path);
			}
			r_list_purge (paths);
			r_cons_printf ("aepc orip\n");
			free (paths);
		}
		}
		break;
	case '?':
		r_core_cmd_help (core, help_msg_abt);
		break;
	case 'j': {
		ut64 addr = r_num_math (core->num, input + 1);
		RAnalBlock *block = r_anal_get_block_at (core->anal, core->offset);
		if (!block) {
			break;
		}
		RList *path = r_anal_block_shortest_path (block, addr);
		PJ *pj = pj_new ();
		if (pj) {
			pj_a (pj);
			if (path) {
				RListIter *it;
				r_list_foreach (path, it, block) {
					pj_n (pj, block->addr);
				}
			}
			pj_end (pj);
			r_cons_println (pj_string (pj));
			pj_free (pj);
		}
		r_list_free (path);
		break;
	}
	case ' ': {
		ut64 addr = r_num_math (core->num, input + 1);
		RAnalBlock *block = r_anal_get_block_at (core->anal, core->offset);
		if (!block) {
			break;
		}
		RList *path = r_anal_block_shortest_path (block, addr);
		if (path) {
			RListIter *it;
			r_list_foreach (path, it, block) {
				r_cons_printf ("0x%08" PFMT64x "\n", block->addr);
			}
			r_list_free (path);
		}
		break;
	}
	case '\0':
		eprintf ("Usage abt?\n");
		break;
	}
}