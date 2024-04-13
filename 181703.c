static void cmd_anal_esil(RCore *core, const char *input) {
	RAnalEsil *esil = core->anal->esil;
	ut64 addr = core->offset;
	ut64 adr ;
	char *n, *n1;
	int off;
	int stacksize = r_config_get_i (core->config, "esil.stack.depth");
	int iotrap = r_config_get_i (core->config, "esil.iotrap");
	int romem = r_config_get_i (core->config, "esil.romem");
	int stats = r_config_get_i (core->config, "esil.stats");
	int noNULL = r_config_get_i (core->config, "esil.noNULL");
	ut64 until_addr = UT64_MAX;
	unsigned int addrsize = r_config_get_i (core->config, "esil.addr.size");

	const char *until_expr = NULL;
	RAnalOp *op = NULL;

	switch (input[0]) {
	case 'p': // "aep"
		switch (input[1]) {
		case 'c': // "aepc"
			if (input[2] == ' ' || input[2] == '=') {
				// seek to this address
				r_core_cmdf (core, "ar PC=%s", r_str_trim_head_ro (input + 3));
				r_core_cmd0 (core, ".ar*");
			} else {
				eprintf ("Missing argument\n");
			}
			break;
		case 0:
			r_anal_pin_list (core->anal);
			break;
		case '-':
			if (input[2]) {
				addr = r_num_math (core->num, input + 2);
			}
			r_anal_pin_unset (core->anal, addr);
			break;
		case ' ':
			r_anal_pin (core->anal, addr, input + 2);
			break;
		default:
			r_core_cmd_help (core, help_msg_aep);
			break;
		}
		break;
	case 'r': // "aer"
		// 'aer' is an alias for 'ar'
		cmd_anal_reg (core, input + 1);
		break;
	case '*':
		// XXX: this is wip, not working atm
		if (core->anal->esil) {
			r_cons_printf ("trap: %d\n", core->anal->esil->trap);
			r_cons_printf ("trap-code: %d\n", core->anal->esil->trap_code);
		} else {
			eprintf ("esil vm not initialized. run `aei`\n");
		}
		break;
	case ' ':
		//r_anal_esil_eval (core->anal, input+1);
		if (!esil && !(core->anal->esil = esil = r_anal_esil_new (stacksize, iotrap, addrsize))) {
			return;
		}
		r_anal_esil_setup (esil, core->anal, romem, stats, noNULL); // setup io
		r_anal_esil_set_pc (esil, core->offset);
		r_anal_esil_parse (esil, input + 1);
		r_anal_esil_dumpstack (esil);
		r_anal_esil_stack_free (esil);
		break;
	case 's': // "aes"
		// "aes" "aeso" "aesu" "aesue"
		// aes -> single step
		// aesb -> single step back
		// aeso -> single step over
		// aesu -> until address
		// aesue -> until esil expression
		switch (input[1]) {
		case '?':
			r_core_cmd0 (core, "ae?~aes");
			break;
		case 'l': // "aesl"
		{
			ut64 pc = r_debug_reg_get (core->dbg, "PC");
			RAnalOp *op = r_core_anal_op (core, pc, R_ANAL_OP_MASK_BASIC | R_ANAL_OP_MASK_HINT);
			// TODO: honor hint
			if (!op) {
				break;
			}
			r_core_esil_step (core, UT64_MAX, NULL, NULL, false);
			r_debug_reg_set (core->dbg, "PC", pc + op->size);
			r_anal_esil_set_pc (esil, pc + op->size);
			r_core_cmd0 (core, ".ar*");
			r_anal_op_free (op);
		} break;
		case 'b': // "aesb"
			if (!r_core_esil_step_back (core)) {
				eprintf ("cannnot step back\n");
			}
			r_core_cmd0 (core, ".ar*");
			break;
		case 'B': // "aesB"
			{
			n = strchr (input + 2, ' ');
			char *n2 = NULL;
			if (n) {
				n = (char *)r_str_trim_head_ro (n + 1);
			}
			if (n) {
				n2 = strchr (n, ' ');
				if (n2) {
					*n2++ = 0;
				}
				ut64 off = r_num_math (core->num, n);
				ut64 nth = n2? r_num_math (core->num, n2): 1;
				cmd_aespc (core, core->offset, off, (int)nth);
			} else {
				eprintf ("Usage: aesB [until-addr] [nth-opcodes] @ [from-addr]\n");
			}
			}
			break;
		case 'u': // "aesu"
			until_expr = NULL;
			until_addr = UT64_MAX;
			if (r_str_endswith (input, "?")) {
				r_core_cmd0 (core, "ae?~aesu");
			} else switch (input[2]) {
			case 'e': // "aesue"
				until_expr = input + 3;
				break;
			case ' ': // "aesu"
				until_addr = r_num_math (core->num, input + 2);
				break;
			case 'o': // "aesuo"
				step_until_optype (core, r_str_trim_head_ro (input + 3));
				break;
			default:
				r_core_cmd0 (core, "ae?~aesu");
				break;
			}
			if (until_expr || until_addr != UT64_MAX) {
				r_core_esil_step (core, until_addr, until_expr, NULL, false);
			}
			r_core_cmd0 (core, ".ar*");
			break;
		case 's': // "aess"
			if (input[2] == 'u') { // "aessu"
				if (input[3] == 'e') {
					until_expr = input + 3;
				} else {
					until_addr = r_num_math (core->num, input + 2);
				}
				r_core_esil_step (core, until_addr, until_expr, NULL, true);
			} else {
				r_core_esil_step (core, UT64_MAX, NULL, NULL, true);
			}
			r_core_cmd0 (core, ".ar*");
			break;
		case 'o': // "aeso"
			if (input[2] == 'u') { // "aesou"
				if (input[3] == 'e') {
					until_expr = input + 3;
				} else {
					until_addr = r_num_math (core->num, input + 2);
				}
				r_core_esil_step (core, until_addr, until_expr, NULL, true);
				r_core_cmd0 (core, ".ar*");
			} else if (!input[2] || input[2] == ' ') { // "aeso [addr]"
				// step over
				op = r_core_anal_op (core, r_reg_getv (core->anal->reg,
					r_reg_get_name (core->anal->reg, R_REG_NAME_PC)), R_ANAL_OP_MASK_BASIC | R_ANAL_OP_MASK_HINT);
				if (op && op->type == R_ANAL_OP_TYPE_CALL) {
					until_addr = op->addr + op->size;
				}
				r_core_esil_step (core, until_addr, until_expr, NULL, false);
				r_anal_op_free (op);
				r_core_cmd0 (core, ".ar*");
			} else {
				eprintf ("Usage: aesou [addr] # step over until given address\n");
			}
			break;
		case 'p': //"aesp"
			n = strchr (input, ' ');
			n1 = n ? strchr (n + 1, ' ') : NULL;
			if ((!n || !n1) || (!(n + 1) || !(n1 + 1))) {
				eprintf ("aesp [offset] [num]\n");
				break;
			}
			adr = r_num_math (core->num, n + 1);
			off = r_num_math (core->num, n1 + 1);
			cmd_aespc (core, adr, -1, off);
			break;
		case ' ':
			n = strchr (input, ' ');
			if (!(n + 1)) {
				r_core_esil_step (core, until_addr, until_expr, NULL, false);
				break;
			}
			off = r_num_math (core->num, n + 1);
			cmd_aespc (core, -1, -1, off);
			break;
		default:
			r_core_esil_step (core, until_addr, until_expr, NULL, false);
			r_core_cmd0 (core, ".ar*");
			break;
		}
		break;
	case 'C': // "aeC"
		if (input[1] == '?') { // "aec?"
			r_core_cmd_help (core, help_msg_aeC);
		} else {
			__core_anal_appcall (core, r_str_trim_head_ro (input + 1));
		}
		break;
	case 'c': // "aec"
		if (input[1] == '?') { // "aec?"
			r_core_cmd_help (core, help_msg_aec);
		} else if (input[1] == 's') { // "aecs"
			const char *pc = r_reg_get_name (core->anal->reg, R_REG_NAME_PC);
			for (;;) {
				if (!r_core_esil_step (core, UT64_MAX, NULL, NULL, false)) {
					break;
				}
				r_core_cmd0 (core, ".ar*");
				addr = r_num_get (core->num, pc);
				op = r_core_anal_op (core, addr, R_ANAL_OP_MASK_BASIC | R_ANAL_OP_MASK_HINT);
				if (!op) {
					break;
				}
				if (op->type == R_ANAL_OP_TYPE_SWI) {
					eprintf ("syscall at 0x%08" PFMT64x "\n", addr);
					break;
				} else if (op->type == R_ANAL_OP_TYPE_TRAP) {
					eprintf ("trap at 0x%08" PFMT64x "\n", addr);
					break;
				}
				r_anal_op_free (op);
				op = NULL;
				if (core->anal->esil->trap || core->anal->esil->trap_code) {
					break;
				}
			}
			if (op) {
				r_anal_op_free (op);
				op = NULL;
			}
		} else if (input[1] == 'c') { // "aecc"
			const char *pc = r_reg_get_name (core->anal->reg, R_REG_NAME_PC);
			for (;;) {
				if (!r_core_esil_step (core, UT64_MAX, NULL, NULL, false)) {
					break;
				}
				r_core_cmd0 (core, ".ar*");
				addr = r_num_get (core->num, pc);
				op = r_core_anal_op (core, addr, R_ANAL_OP_MASK_BASIC);
				if (!op) {
					break;
				}
				if (op->type == R_ANAL_OP_TYPE_CALL || op->type == R_ANAL_OP_TYPE_UCALL) {
					eprintf ("call at 0x%08" PFMT64x "\n", addr);
					break;
				}
				r_anal_op_free (op);
				op = NULL;
				if (core->anal->esil->trap || core->anal->esil->trap_code) {
					break;
				}
			}
			if (op) {
				r_anal_op_free (op);
			}
		} else {
			// "aec"  -> continue until ^C
			// "aecu" -> until address
			// "aecue" -> until esil expression
			if (input[1] == 'u' && input[2] == 'e') {
				until_expr = input + 3;
			} else if (input[1] == 'u') {
				until_addr = r_num_math (core->num, input + 2);
			} else {
				until_expr = "0";
			}
			r_core_esil_step (core, until_addr, until_expr, NULL, false);
			r_core_cmd0 (core, ".ar*");
		}
		break;
	case 'i': // "aei"
		switch (input[1]) {
		case 's': // "aeis"
		case 'm': // "aeim"
			cmd_esil_mem (core, input + 2);
			break;
		case 'p': // "aeip" // initialize pc = $$
			r_core_cmd0 (core, "ar PC=$$");
			break;
		case '?':
			cmd_esil_mem (core, "?");
			break;
		case '-':
			if (esil) {
				sdb_reset (esil->stats);
			}
			r_anal_esil_free (esil);
			core->anal->esil = NULL;
			break;
		case 0: //lolololol
			r_anal_esil_free (esil);
			// reinitialize
			{
				const char *pc = r_reg_get_name (core->anal->reg, R_REG_NAME_PC);
				if (pc && r_reg_getv (core->anal->reg, pc) == 0LL) {
					r_core_cmd0 (core, "ar PC=$$");
				}
			}
			if (!(esil = core->anal->esil = r_anal_esil_new (stacksize, iotrap, addrsize))) {
				return;
			}
			r_anal_esil_setup (esil, core->anal, romem, stats, noNULL); // setup io
			esil->verbose = (int)r_config_get_i (core->config, "esil.verbose");
			/* restore user settings for interrupt handling */
			{
				const char *s = r_config_get (core->config, "cmd.esil.intr");
				if (s) {
					char *my = strdup (s);
					if (my) {
						r_config_set (core->config, "cmd.esil.intr", my);
						free (my);
					}
				}
			}
			break;
		}
		break;
	case 'k': // "aek"
		switch (input[1]) {
		case '\0':
			input = "123*";
			/* fall through */
		case ' ':
			if (esil && esil->stats) {
				char *out = sdb_querys (esil->stats, NULL, 0, input + 2);
				if (out) {
					r_cons_println (out);
					free (out);
				}
			} else {
				eprintf ("esil.stats is empty. Run 'aei'\n");
			}
			break;
		case '-':
			if (esil) {
				sdb_reset (esil->stats);
			}
			break;
		}
		break;
	case 'l': // ael commands
		switch (input[1]) {
		case 'i': // aeli interrupts
			switch (input[2]) {
			case ' ': // "aeli" with arguments
				if (!r_anal_esil_load_interrupts_from_lib (esil, input + 3)) {
					eprintf ("Failed to load interrupts from '%s'.", input + 3);
				}
				break;
			case 0: // "aeli" with no args
				if (esil && esil->interrupts) {
					dict_foreach (esil->interrupts, _aeli_iter, NULL);
				}
				break;
			case 'r': // "aelir"
				if (esil && esil->interrupts) {
					RAnalEsilInterrupt* interrupt = dict_getu (esil->interrupts, r_num_math (core->num, input + 3));
					r_anal_esil_interrupt_free (esil, interrupt);
				}
				break;

				// TODO: display help?
			}
		}
		break;
	case 'g': // "aeg"
		if (input[1] == 'v') {
			r_core_cmd0 (core, ".aeg;agg");
		} else if (input[1] == ' ') {
			r_core_anal_esil_graph (core, input + 2);
		} else { // "*"
			RAnalOp *aop = r_core_anal_op (core, core->offset, R_ANAL_OP_MASK_ESIL);
			if (aop) {
				const char *esilstr = r_strbuf_get (&aop->esil);
				if (esilstr) {
					r_core_anal_esil_graph (core, esilstr);
				}
			}
		}
		break;
	case 'b': // "aeb"
		// ab~ninstr[1]
		r_core_cmdf (core, "aesp `ab~addr[1]` `ab~ninstr[1]`");
		break;
	case 'f': // "aef"
		if (input[1] == 'a') { // "aefa"
			r_anal_aefa (core, r_str_trim_head_ro (input + 2));
		} else { // This should be aefb -> because its emulating all the bbs
		RListIter *iter;
		RAnalBlock *bb;
		RAnalFunction *fcn = r_anal_get_fcn_in (core->anal,
							core->offset, R_ANAL_FCN_TYPE_FCN | R_ANAL_FCN_TYPE_SYM);
		if (fcn) {
			// emulate every instruction in the function recursively across all the basic blocks
			r_list_foreach (fcn->bbs, iter, bb) {
				ut64 pc = bb->addr;
				ut64 end = bb->addr + bb->size;
				RAnalOp op;
				int ret, bbs = end - pc;
				if (bbs < 1 || bbs > 0xfffff) {
					eprintf ("Invalid block size\n");
				}
		//		eprintf ("[*] Emulating 0x%08"PFMT64x" basic block 0x%08" PFMT64x " - 0x%08" PFMT64x "\r[", fcn->addr, pc, end);
				ut8 *buf = calloc (1, bbs + 1);
				if (!buf) {
					break;
				}
				r_io_read_at (core->io, pc, buf, bbs);
				int left;
				while (pc < end) {
					left = R_MIN (end - pc, 32);
					r_asm_set_pc (core->assembler, pc);
					ret = r_anal_op (core->anal, &op, pc, buf + pc - bb->addr, left, R_ANAL_OP_MASK_HINT | R_ANAL_OP_MASK_ESIL); // read overflow
					if (op.type == R_ANAL_OP_TYPE_RET) {
						break;
					}
					if (ret) {
						r_reg_set_value_by_role (core->anal->reg, R_REG_NAME_PC, pc);
						r_anal_esil_parse (esil, R_STRBUF_SAFEGET (&op.esil));
						r_anal_esil_dumpstack (esil);
						r_anal_esil_stack_free (esil);
						pc += op.size;
					} else {
						pc += 4; // XXX
					}
				}
				free (buf);
			}
		} else {
			eprintf ("Cannot find function at 0x%08" PFMT64x "\n", core->offset);
		}
	} break;
	case 't': // "aet"
		switch (input[1]) {
		case 'r': // "aetr"
		{
			// anal ESIL to REIL.
			RAnalEsil *esil = r_anal_esil_new (stacksize, iotrap, addrsize);
			if (!esil) {
				return;
			}
			r_anal_esil_to_reil_setup (esil, core->anal, romem, stats);
			r_anal_esil_set_pc (esil, core->offset);
			r_anal_esil_parse (esil, input + 2);
			r_anal_esil_dumpstack (esil);
			r_anal_esil_free (esil);
			break;
		}
		case 's': // "aets"
			switch (input[2]) {
			case 0:
				r_anal_esil_session_list (esil);
				break;
			case '+':
				r_anal_esil_session_add (esil);
				break;
			default:
				r_core_cmd_help (core, help_msg_aets);
				break;
			}
			break;
		default:
			eprintf ("Unknown command. Use `aetr`.\n");
			break;
		}
		break;
	case 'A': // "aeA"
		if (input[1] == '?') {
			r_core_cmd_help (core, help_msg_aea);
		} else if (input[1] == 'r') {
			cmd_aea (core, 1 + (1<<1), core->offset, r_num_math (core->num, input+2));
		} else if (input[1] == 'w') {
			cmd_aea (core, 1 + (1<<2), core->offset, r_num_math (core->num, input+2));
		} else if (input[1] == 'n') {
			cmd_aea (core, 1 + (1<<3), core->offset, r_num_math (core->num, input+2));
		} else if (input[1] == 'j') {
			cmd_aea (core, 1 + (1<<4), core->offset, r_num_math (core->num, input+2));
		} else if (input[1] == '*') {
			cmd_aea (core, 1 + (1<<5), core->offset, r_num_math (core->num, input+2));
		} else if (input[1] == 'f') {
			RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, -1);
			if (fcn) {
				cmd_aea (core, 1, r_anal_function_min_addr (fcn), r_anal_function_linear_size (fcn));
			}
		} else {
			cmd_aea (core, 1, core->offset, (int)r_num_math (core->num, input+2));
		}
		break;
	case 'a': // "aea"
		{
		RReg *reg = core->anal->reg;
		ut64 pc = r_reg_getv (reg, "PC");
		RAnalOp *op = r_core_anal_op (core, pc, 0);
		if (!op) {
			break;
		}
		ut64 newPC = core->offset + op->size;
		r_reg_setv (reg, "PC", newPC);
		if (input[1] == '?') {
			r_core_cmd_help (core, help_msg_aea);
		} else if (input[1] == 'r') {
			cmd_aea (core, 1<<1, core->offset, r_num_math (core->num, input+2));
		} else if (input[1] == 'w') {
			cmd_aea (core, 1<<2, core->offset, r_num_math (core->num, input+2));
		} else if (input[1] == 'n') {
			cmd_aea (core, 1<<3, core->offset, r_num_math (core->num, input+2));
		} else if (input[1] == 'j') {
			cmd_aea (core, 1<<4, core->offset, r_num_math (core->num, input+2));
		} else if (input[1] == '*') {
			cmd_aea (core, 1<<5, core->offset, r_num_math (core->num, input+2));
		} else if (input[1] == 'b') { // "aeab"
			bool json = input[2] == 'j';
			int a = json? 3: 2;
			ut64 addr = (input[a] == ' ')? r_num_math (core->num, input + a): core->offset;
			RList *l = r_anal_get_blocks_in (core->anal, addr);
			RAnalBlock *b;
			RListIter *iter;
			r_list_foreach (l, iter, b) {
				int mode = json? (1<<4): 1;
				cmd_aea (core, mode, b->addr, b->size);
				break;
			}
		} else if (input[1] == 'f') {
			RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, -1);
                        // "aeafj"
			if (fcn) {
				switch (input[2]) {
				case 'j': // "aeafj"
					cmd_aea (core, 1<<4, r_anal_function_min_addr (fcn), r_anal_function_linear_size (fcn));
					break;
				default:
					cmd_aea (core, 1, r_anal_function_min_addr (fcn), r_anal_function_linear_size (fcn));
					break;
				}
				break;
			}
		} else if (input[1] == 'b') { // "aeab"
			RAnalBlock *bb = r_anal_bb_from_offset (core->anal, core->offset);
			if (bb) {
				switch (input[2]) {
				case 'j': // "aeabj"
					cmd_aea (core, 1<<4, bb->addr, bb->size);
					break;
				default:
					cmd_aea (core, 1, bb->addr, bb->size);
					break;
				}
			}
		} else {
			const char *arg = input[1]? input + 2: "";
			ut64 len = r_num_math (core->num, arg);
			cmd_aea (core, 0, core->offset, len);
		}
		r_reg_setv (reg, "PC", pc);
}
		break;
	case 'x': { // "aex"
		char *hex;
		int ret, bufsz;

		input = r_str_trim_head_ro (input + 1);
		hex = strdup (input);
		if (!hex) {
			break;
		}

		RAnalOp aop = R_EMPTY;
		bufsz = r_hex_str2bin (hex, (ut8*)hex);
		ret = r_anal_op (core->anal, &aop, core->offset,
			(const ut8*)hex, bufsz, R_ANAL_OP_MASK_ESIL);
		if (ret>0) {
			const char *str = R_STRBUF_SAFEGET (&aop.esil);
			char *str2 = r_str_newf (" %s", str);
			cmd_anal_esil (core, str2);
			free (str2);
		}
		r_anal_op_fini (&aop);
		break;
	}
	case '?': // "ae?"
		if (input[1] == '?') {
			r_core_cmd_help (core, help_detail_ae);
			break;
		}
		/* fallthrough */
	default:
		r_core_cmd_help (core, help_msg_ae);
		break;
	}
}