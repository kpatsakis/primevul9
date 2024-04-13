static void cmd_anal_hint(RCore *core, const char *input) {
	switch (input[0]) {
	case '?':
		if (input[1]) {
			ut64 addr = r_num_math (core->num, input + 1);
			r_core_anal_hint_print (core->anal, addr, 0);
		} else {
			r_core_cmd_help (core, help_msg_ah);
		}
		break;
	case '.': // "ah."
		r_core_anal_hint_print (core->anal, core->offset, 0);
		break;
	case 'a': // "aha" set arch
		if (input[1] == ' ') {
			char *ptr = strdup (input + 2);
			r_str_word_set0 (ptr);
			const char *arch = r_str_word_get0 (ptr, 0);
			r_anal_hint_set_arch (core->anal, core->offset, !arch || strcmp (arch, "0") == 0 ? NULL : arch);
			free (ptr);
		} else if (input[1] == '-') {
			r_anal_hint_unset_arch (core->anal, core->offset);
		} else {
			eprintf ("Missing argument\n");
		}
		break;
	case 'o': // "aho"
		if (input[1] == ' ') {
			const char *arg = r_str_trim_head_ro (input + 1);
			int type = r_anal_optype_from_string (arg);
			r_anal_hint_set_type (core->anal, core->offset, type);
		} else {
			eprintf ("Usage: aho [type] # can be mov, jmp, call, ...\n");
		}
		break;
	case 'b': // "ahb" set bits
		if (input[1] == ' ') {
			char *ptr = strdup (input + 2);
			int bits;
			int i = r_str_word_set0 (ptr);
			if (i == 2) {
				r_num_math (core->num, r_str_word_get0 (ptr, 1));
			}
			bits = r_num_math (core->num, r_str_word_get0 (ptr, 0));
			r_anal_hint_set_bits (core->anal, core->offset, bits);
			free (ptr);
		}  else if (input[1] == '-') {
			r_anal_hint_unset_bits (core->anal, core->offset);
		} else {
			eprintf ("Missing argument\n");
		}
		break;
	case 'i': // "ahi"
		if (input[1] == '?') {
			r_core_cmd_help (core, help_msg_ahi);
		} else if (isdigit (input[1])) {
			r_anal_hint_set_nword (core->anal, core->offset, input[1] - '0');
			input++;
		} else if (input[1] == '-') { // "ahi-"
			r_anal_hint_set_immbase (core->anal, core->offset, 0);
		}
		if (input[1] == ' ') {
			// You can either specify immbase with letters, or numbers
			const int base =
				(input[2] == 's') ? 1 :
				(input[2] == 'b') ? 2 :
				(input[2] == 'p') ? 3 :
				(input[2] == 'o') ? 8 :
				(input[2] == 'd') ? 10 :
				(input[2] == 'h') ? 16 :
				(input[2] == 'i') ? 32 : // ip address
				(input[2] == 'S') ? 80 : // syscall
				(int) r_num_math (core->num, input + 1);
			r_anal_hint_set_immbase (core->anal, core->offset, base);
		} else if (input[1] != '?' && input[1] != '-') {
			eprintf ("|ERROR| Usage: ahi [base]\n");
		}
		break;
	case 'h': // "ahh"
		if (input[1] == '-') {
			r_anal_hint_unset_high (core->anal, core->offset);
		} else if (input[1] == ' ') {
			r_anal_hint_set_high (core->anal, r_num_math (core->num, input + 1));
		} else {
			r_anal_hint_set_high (core->anal, core->offset);
		}
		break;
	case 'c': // "ahc"
		if (input[1] == ' ') {
			r_anal_hint_set_jump (
				core->anal, core->offset,
				r_num_math (core->num, input + 1));
		} else if (input[1] == '-') {
			r_anal_hint_unset_jump (core->anal, core->offset);
		}
		break;
	case 'f': // "ahf"
		if (input[1] == ' ') {
			r_anal_hint_set_fail (
				core->anal, core->offset,
				r_num_math (core->num, input + 1));
		} else if (input[1] == '-') {
			r_anal_hint_unset_fail (core->anal, core->offset);
		}
		break;
	case 'F': // "ahF" set stackframe size
		if (input[1] == ' ') {
			r_anal_hint_set_stackframe (
				core->anal, core->offset,
				r_num_math (core->num, input + 1));
		} else if (input[1] == '-') {
			r_anal_hint_unset_stackframe (core->anal, core->offset);
		}
		break;
	case 's': // "ahs" set size (opcode length)
		if (input[1] == ' ') {
			r_anal_hint_set_size (core->anal, core->offset, atoi (input + 1));
		} else if (input[1] == '-') {
			r_anal_hint_unset_size (core->anal, core->offset);
		} else {
			eprintf ("Usage: ahs 16\n");
		}
		break;
	case 'S': // "ahS" set asm.syntax
		if (input[1] == ' ') {
			r_anal_hint_set_syntax (core->anal, core->offset, input + 2);
		} else if (input[1] == '-') {
			r_anal_hint_unset_syntax (core->anal, core->offset);
		} else {
			eprintf ("Usage: ahS att\n");
		}
		break;
	case 'd': // "ahd" set opcode string
		if (input[1] == ' ') {
			r_anal_hint_set_opcode (core->anal, core->offset, input + 2);
		} else if (input[1] == '-') {
			r_anal_hint_unset_opcode (core->anal, core->offset);
		} else {
			eprintf ("Usage: ahd popall\n");
		}
		break;
	case 'e': // "ahe" set ESIL string
		if (input[1] == ' ') {
			r_anal_hint_set_esil (core->anal, core->offset, input + 2);
		} else if (input[1] == '-') {
			r_anal_hint_unset_esil (core->anal, core->offset);
		} else {
			eprintf ("Usage: ahe r0,pc,=\n");
		}
		break;
#if 0
	case 'e': // set endian
		if (input[1] == ' ') {
			r_anal_hint_set_opcode (core->anal, core->offset, atoi (input + 1));
		} else if (input[1] == '-') {
			r_anal_hint_unset_opcode (core->anal, core->offset);
		}
		break;
#endif
	case 'p': // "ahp"
		if (input[1] == ' ') {
			r_anal_hint_set_pointer (core->anal, core->offset, r_num_math (core->num, input + 1));
		} else if (input[1] == '-') { // "ahp-"
			r_anal_hint_unset_pointer (core->anal, core->offset);
		}
		break;
	case 'r': // "ahr"
		if (input[1] == ' ') {
			r_anal_hint_set_ret (core->anal, core->offset, r_num_math (core->num, input + 1));
		} else if (input[1] == '-') { // "ahr-"
			r_anal_hint_unset_ret (core->anal, core->offset);
		}
	case '*': // "ah*"
	case 'j': // "ahj"
	case '\0': // "ah"
		if (input[0] && input[1] == ' ') {
			char *ptr = strdup (r_str_trim_head_ro (input + 2));
			r_str_word_set0 (ptr);
			ut64 addr = r_num_math (core->num, r_str_word_get0 (ptr, 0));
			r_core_anal_hint_print (core->anal, addr, input[0]);
			free (ptr);
		} else {
			r_core_anal_hint_list (core->anal, input[0]);
		}
		break;
	case 'v': // "ahv"
		if (input[1] == ' ') {
			r_anal_hint_set_val (
				core->anal, core->offset,
				r_num_math (core->num, input + 1));
		} else if (input[1] == '-') {
			r_anal_hint_unset_val (core->anal, core->offset);
		}
		break;
	case '-': // "ah-"
		if (input[1]) {
			if (input[1] == '*') {
				r_anal_hint_clear (core->anal);
			} else {
				char *ptr = strdup (r_str_trim_head_ro (input + 1));
				ut64 addr;
				int size = 1;
				int i = r_str_word_set0 (ptr);
				if (i == 2) {
					size = r_num_math (core->num, r_str_word_get0 (ptr, 1));
				}
				const char *a0 = r_str_word_get0 (ptr, 0);
				if (a0 && *a0) {
					addr = r_num_math (core->num, a0);
				} else {
					addr = core->offset;
				}
				r_anal_hint_del (core->anal, addr, size);
				free (ptr);
			}
		} else {
			r_anal_hint_clear (core->anal);
		} break;
	case 't': // "aht"
		switch (input[1]) {
		case 's': { // "ahts"
			char *off = strdup (input + 2);
			r_str_trim (off);
			int toff = r_num_math (NULL, off);
			if (toff) {
				RList *typeoffs = r_type_get_by_offset (core->anal->sdb_types, toff);
				RListIter *iter;
				char *ty;
				r_list_foreach (typeoffs, iter, ty) {
					r_cons_printf ("%s\n", ty);
				}
				r_list_free (typeoffs);
			}
			free (off);
			break;
		}
		case ' ': {
			// r_anal_hint_set_opcode (core->anal, core->offset, input + 2);
			const char *off = NULL;
			char *type = strdup (r_str_trim_head_ro (input + 2));
			char *idx = strchr (type, ' ');
			if (idx) {
				*idx++ = 0;
				off = idx;
			}
			char *ptr = strchr (type, '=');
			ut64 offimm = 0;
			int i = 0;
			ut64 addr;

			if (ptr) {
				*ptr++ = 0;
				r_str_trim (ptr);
				if (ptr && *ptr) {
					addr = r_num_math (core->num, ptr);
				} else {
					eprintf ("address is unvalid\n");
					free (type);
					break;
				}
			} else {
				addr = core->offset;
			}
			r_str_trim (type);
			RAsmOp asmop;
			RAnalOp op = { 0 };
			ut8 code[128] = { 0 };
			(void)r_io_read_at (core->io, core->offset, code, sizeof (code));
			r_asm_set_pc (core->assembler, addr);
			(void)r_asm_disassemble (core->assembler, &asmop, code, core->blocksize);
			int ret = r_anal_op (core->anal, &op, core->offset, code, core->blocksize, R_ANAL_OP_MASK_VAL);
			if (ret >= 0) {
				// HACK: Just convert only the first imm seen
				for (i = 0; i < 3; i++) {
					if (op.src[i]) {
						if (op.src[i]->imm) {
							offimm = op.src[i]->imm;
						} else if (op.src[i]->delta) {
							offimm = op.src[i]->delta;
						}
					}
				}
				if (!offimm && op.dst) {
					if (op.dst->imm) {
						offimm = op.dst->imm;
					} else if (op.dst->delta) {
						offimm = op.dst->delta;
					}
				}
				if (offimm != 0) {
					if (off) {
						offimm += r_num_math (NULL, off);
					}
					// TODO: Allow to select from multiple choices
					RList *otypes = r_type_get_by_offset (core->anal->sdb_types, offimm);
					RListIter *iter;
					char *otype = NULL;
					r_list_foreach (otypes, iter, otype) {
						// TODO: I don't think we should silently error, it is confusing
						if (!strcmp (type, otype)) {
							//eprintf ("Adding type offset %s\n", type);
							r_type_link_offset (core->anal->sdb_types, type, addr);
							r_anal_hint_set_offset (core->anal, addr, otype);
							break;
						}
					}
					if (!otype) {
						eprintf ("wrong type for opcode offset\n");
					}
					r_list_free (otypes);
				}
			}
			r_anal_op_fini (&op);
			free (type);
		} break;
		case '?':
			r_core_cmd_help (core, help_msg_aht);
			break;
		}
	}
}