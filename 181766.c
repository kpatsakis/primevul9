static void core_anal_bytes(RCore *core, const ut8 *buf, int len, int nops, int fmt) {
	int stacksize = r_config_get_i (core->config, "esil.stack.depth");
	bool iotrap = r_config_get_i (core->config, "esil.iotrap");
	bool romem = r_config_get_i (core->config, "esil.romem");
	bool stats = r_config_get_i (core->config, "esil.stats");
	bool be = core->print->big_endian;
	bool use_color = core->print->flags & R_PRINT_FLAGS_COLOR;
	core->parser->relsub = r_config_get_i (core->config, "asm.relsub");
	int ret, i, j, idx, size;
	const char *color = "";
	const char *esilstr;
	const char *opexstr;
	RAnalHint *hint;
	RAnalEsil *esil = NULL;
	RAsmOp asmop;
	RAnalOp op = {0};
	ut64 addr;
	PJ *pj = NULL;
	unsigned int addrsize = r_config_get_i (core->config, "esil.addr.size");
	int totalsize = 0;

	// Variables required for setting up ESIL to REIL conversion
	if (use_color) {
		color = core->cons->context->pal.label;
	}
	switch (fmt) {
	case 'j': {
		pj = pj_new ();
		if (!pj) {
			break;
		}
		pj_a (pj);
	} break;
	case 'r':
		// Setup for ESIL to REIL conversion
		esil = r_anal_esil_new (stacksize, iotrap, addrsize);
		if (!esil) {
			return;
		}
		r_anal_esil_to_reil_setup (esil, core->anal, romem, stats);
		r_anal_esil_set_pc (esil, core->offset);
		break;
	}
	for (i = idx = ret = 0; idx < len && (!nops || (nops && i < nops)); i++, idx += ret) {
		addr = core->offset + idx;
		// TODO: use more anal hints
		hint = r_anal_hint_get (core->anal, addr);
		r_asm_set_pc (core->assembler, addr);
		ret = r_anal_op (core->anal, &op, addr, buf + idx, len - idx,
			R_ANAL_OP_MASK_ESIL | R_ANAL_OP_MASK_OPEX | R_ANAL_OP_MASK_HINT);
		(void)r_asm_disassemble (core->assembler, &asmop, buf + idx, len - idx);
		esilstr = R_STRBUF_SAFEGET (&op.esil);
		opexstr = R_STRBUF_SAFEGET (&op.opex);
		char *mnem = strdup (r_asm_op_get_asm (&asmop));
		char *sp = strchr (mnem, ' ');
		if (sp) {
			*sp = 0;
			if (op.prefix) {
				char *arg = strdup (sp + 1);
				char *sp = strchr (arg, ' ');
				if (sp) {
					*sp = 0;
				}
				free (mnem);
				mnem = arg;
			}
		}
		if (ret < 1 && fmt != 'd') {
			eprintf ("Oops at 0x%08" PFMT64x " (", core->offset + idx);
			for (i = idx, j = 0; i < core->blocksize && j < 3; i++, j++) {
				eprintf ("%02x ", buf[i]);
			}
			eprintf ("...)\n");
			free (mnem);
			break;
		}
		size = (hint && hint->size)? hint->size: op.size;
		if (fmt == 'd') {
			char *opname = strdup (r_asm_op_get_asm (&asmop));
			if (opname) {
				r_str_split (opname, ' ');
				char *d = r_asm_describe (core->assembler, opname);
				if (d && *d) {
					r_cons_printf ("%s: %s\n", opname, d);
					free (d);
				} else {
					eprintf ("Unknown opcode\n");
				}
				free (opname);
			}
		} else if (fmt == 'e') {
			if (*esilstr) {
				if (use_color) {
					r_cons_printf ("%s0x%" PFMT64x Color_RESET " %s\n", color, core->offset + idx, esilstr);
				} else {
					r_cons_printf ("0x%" PFMT64x " %s\n", core->offset + idx, esilstr);
				}
			}
		} else if (fmt == 's') {
			totalsize += op.size;
		} else if (fmt == 'j') {
			char strsub[128] = { 0 };
			// pc+33
			r_parse_varsub (core->parser, NULL,
				core->offset + idx,
				asmop.size, r_asm_op_get_asm (&asmop),
				strsub, sizeof (strsub));
				ut64 killme = UT64_MAX;
				if (r_io_read_i (core->io, op.ptr, &killme, op.refptr, be)) {
					core->parser->relsub_addr = killme;
				}
			// 0x33->sym.xx
			char *p = strdup (strsub);
			if (p) {
				r_parse_filter (core->parser, addr, core->flags, hint, p,
						strsub, sizeof (strsub), be);
				free (p);
			}
			pj_o (pj);
			pj_ks (pj, "opcode", r_asm_op_get_asm (&asmop));
			if (!*strsub) {
				r_str_ncpy (strsub, r_asm_op_get_asm (&asmop), sizeof (strsub) -1 );
			}
			{
				RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, addr, 0);
				if (fcn) {
					r_parse_varsub (core->parser, fcn, addr, asmop.size,
							strsub, strsub, sizeof (strsub));
				}
			}
			pj_ks (pj, "disasm", strsub);
			// apply pseudo if needed
			{
				char *pseudo = calloc (128 + strlen (strsub), 3);
				r_parse_parse (core->parser, strsub, pseudo);
				if (pseudo && *pseudo) {
					pj_ks (pj, "pseudo", pseudo);
				}
				free (pseudo);
			}
			{
				char *opname = strdup (strsub);
				char *sp = strchr (opname, ' ');
				if (sp) {
					*sp = 0;
				}
				char *d = r_asm_describe (core->assembler, opname);
				if (d && *d) {
					pj_ks (pj, "description", d);
				}
				free (d);
				free (opname);
			}
			pj_ks (pj, "mnemonic", mnem);
			{
				ut8 *mask = r_anal_mask (core->anal, len - idx, buf + idx, core->offset + idx);
				char *maskstr = r_hex_bin2strdup (mask, size);
				pj_ks (pj, "mask", maskstr);
				free (mask);
				free (maskstr);
			}
			if (hint && hint->opcode) {
				pj_ks (pj, "ophint", hint->opcode);
			}
			if (hint && hint->jump != UT64_MAX) {
				op.jump = hint->jump;
			}
			if (hint && hint->fail != UT64_MAX) {
				op.fail = hint->fail;
			}
			if (op.jump != UT64_MAX) {
				pj_kn (pj, "jump", op.jump);
			}
			if (op.fail != UT64_MAX) {
				pj_kn (pj, "fail", op.fail);
			}
			const char *jesil = (hint && hint->esil) ? hint->esil: esilstr;
			if (jesil && *jesil) {
				pj_ks (pj, "esil", jesil);
			}
			pj_kb (pj, "sign", op.sign);
			pj_kn (pj, "prefix", op.prefix);
			pj_ki (pj, "id", op.id);
			if (opexstr && *opexstr) {
				pj_k (pj, "opex");
				pj_j (pj, opexstr);
			}
			pj_kn (pj, "addr", core->offset + idx);
			{
				char *bytes = r_hex_bin2strdup (buf + idx, ret);
				pj_ks (pj, "bytes", bytes);
				free (bytes);
			}
			if (op.val != UT64_MAX) {
				pj_kn (pj, "val", op.val);
			}
			if (op.disp && op.disp != UT64_MAX) {
				pj_kn (pj, "disp", op.disp);
			}
			if (op.ptr != UT64_MAX) {
				pj_kn (pj, "ptr", op.ptr);
			}
			pj_ki (pj, "size", size);
			pj_ks (pj, "type", r_anal_optype_to_string (op.type));
			{
				const char *datatype = r_anal_datatype_to_string (op.datatype);
				if (datatype) {
					pj_ks (pj, "datatype", datatype);
				}

			}
			if (op.reg) {
				pj_ks (pj, "reg", op.reg);
			}
			if (op.ireg) {
				pj_ks (pj, "ireg", op.ireg);
			}
			pj_ki (pj, "scale", op.scale);
			if (op.refptr != -1) {
				pj_ki (pj, "refptr", op.refptr);
			}
			pj_ki (pj, "cycles", op.cycles);
			pj_ki (pj, "failcycles", op.failcycles);
			pj_ki (pj, "delay", op.delay);
			const char *p1 = r_anal_stackop_tostring (op.stackop);
			if (strcmp (p1, "null")) {
				pj_ks (pj, "stack", p1);
			}
			pj_kn (pj, "stackptr", op.stackptr);
			const char *arg = (op.type & R_ANAL_OP_TYPE_COND)
				? r_anal_cond_tostring (op.cond): NULL;
			if (arg) {
				pj_ks (pj, "cond", arg);
			}
			pj_ks (pj, "family", r_anal_op_family_to_string (op.family));
			pj_end (pj);
		} else if (fmt == 'r') {
			if (*esilstr) {
				if (use_color) {
					r_cons_printf ("%s0x%" PFMT64x Color_RESET "\n", color, core->offset + idx);
				} else {
					r_cons_printf ("0x%" PFMT64x "\n", core->offset + idx);
				}
				r_anal_esil_parse (esil, esilstr);
				r_anal_esil_dumpstack (esil);
				r_anal_esil_stack_free (esil);
			}
		} else {
		char disasm[128] = { 0 };
		r_parse_varsub (core->parser, NULL,
			core->offset + idx,
			asmop.size, r_asm_op_get_asm (&asmop),
			disasm, sizeof (disasm));
		ut64 killme = UT64_MAX;
		if (r_io_read_i (core->io, op.ptr, &killme, op.refptr, be)) {
			core->parser->relsub_addr = killme;
		}
		char *p = strdup (disasm);
		if (p) {
			r_parse_filter (core->parser, addr, core->flags, hint, p,
				disasm, sizeof (disasm), be);
			free (p);
		}
#define printline(k, fmt, arg)\
	{ \
		if (use_color)\
			r_cons_printf ("%s%s: " Color_RESET, color, k);\
		else\
			r_cons_printf ("%s: ", k);\
		if (fmt) r_cons_printf (fmt, arg);\
	}
			printline ("address", "0x%" PFMT64x "\n", core->offset + idx);
			printline ("opcode", "%s\n", r_asm_op_get_asm (&asmop));
			if (!*disasm) {
				r_str_ncpy (disasm, r_asm_op_get_asm (&asmop), sizeof (disasm) - 1);
			}
			{
				RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, addr, 0);
				if (fcn) {
					r_parse_varsub (core->parser, fcn, addr, asmop.size,
							disasm, disasm, sizeof (disasm));
				}
			}
			printline ("disasm", "%s\n", disasm);
			{
				char *pseudo = calloc (128 + strlen (disasm), 3);
				r_parse_parse (core->parser, disasm, pseudo);
				if (pseudo && *pseudo) {
					printline ("pseudo", "%s\n", pseudo);
				}
				free (pseudo);
			}
			printline ("mnemonic", "%s\n", mnem);
			{
				char *opname = strdup (disasm);
				char *sp = strchr (opname, ' ');
				if (sp) {
					*sp = 0;
				}
				char *d = r_asm_describe (core->assembler, opname);
				if (d && *d) {
					printline ("description", "%s\n", d);
				}
				free (d);
				free (opname);
			}
			{
				ut8 *mask = r_anal_mask (core->anal, len - idx, buf + idx, core->offset + idx);
				char *maskstr = r_hex_bin2strdup (mask, size);
				printline ("mask", "%s\n", maskstr);
				free (mask);
				free (maskstr);
			}
			if (hint) {
				if (hint->opcode) {
					printline ("ophint", "%s\n", hint->opcode);
				}
			}
			printline ("prefix", "%" PFMT64u "\n", op.prefix);
			printline ("id", "%d\n", op.id);
#if 0
// no opex here to avoid lot of tests broken..and having json in here is not much useful imho
			if (opexstr && *opexstr) {
				printline ("opex", "%s\n", opexstr);
			}
#endif
			printline ("bytes", NULL, 0);
			int minsz = R_MIN (len, size);
			minsz = R_MAX (minsz, 0);
			for (j = 0; j < minsz; j++) {
				ut8 ch = ((j + idx - 1) > minsz)? 0xff: buf[j + idx];
				r_cons_printf ("%02x", ch);
			}
			r_cons_newline ();
			if (op.val != UT64_MAX) {
				printline ("val", "0x%08" PFMT64x "\n", op.val);
			}
			if (op.ptr != UT64_MAX) {
				printline ("ptr", "0x%08" PFMT64x "\n", op.ptr);
			}
			if (op.disp && op.disp != UT64_MAX) {
				printline ("disp", "0x%08" PFMT64x "\n", op.disp);
			}
			if (op.refptr != -1) {
				printline ("refptr", "%d\n", op.refptr);
			}
			printline ("size", "%d\n", size);
			printline ("sign", "%s\n", r_str_bool (op.sign));
			printline ("type", "%s\n", r_anal_optype_to_string (op.type));
			const char *datatype = r_anal_datatype_to_string (op.datatype);
			if (datatype) {
				printline ("datatype", "%s\n", datatype);
			}
			printline ("cycles", "%d\n", op.cycles);
			if (op.failcycles) {
				printline ("failcycles", "%d\n", op.failcycles);
			}
			if (op.type2) {
				printline ("type2", "0x%x\n", op.type2);
			}
			if (op.reg) {
				printline ("reg", "%s\n", op.reg);
			}
			if (op.ireg) {
				printline ("ireg", "%s\n", op.ireg);
			}
			if (op.scale) {
				printline ("scale", "%d\n", op.scale);
			}
			if (hint && hint->esil) {
				printline ("esil", "%s\n", hint->esil);
			} else if (*esilstr) {
				printline ("esil", "%s\n", esilstr);
			}
			if (hint && hint->jump != UT64_MAX) {
				op.jump = hint->jump;
			}
			if (op.jump != UT64_MAX) {
				printline ("jump", "0x%08" PFMT64x "\n", op.jump);
			}
			if (op.direction != 0) {
				const char * dir = op.direction == 1 ? "read"
					: op.direction == 2 ? "write"
					: op.direction == 4 ? "exec"
					: op.direction == 8 ? "ref": "none";
				printline ("direction", "%s\n", dir);
			}
			if (hint && hint->fail != UT64_MAX) {
				op.fail = hint->fail;
			}
			if (op.fail != UT64_MAX) {
				printline ("fail", "0x%08" PFMT64x "\n", op.fail);
			}
			if (op.delay) {
				printline ("delay", "%d\n", op.delay);
			}
			{
				const char *arg = (op.type & R_ANAL_OP_TYPE_COND)?  r_anal_cond_tostring (op.cond): NULL;
				if (arg) {
					printline ("cond", "%s\n", arg);
				}
			}
			printline ("family", "%s\n", r_anal_op_family_to_string (op.family));
			if (op.stackop != R_ANAL_STACK_NULL) {
				printline ("stackop", "%s\n", r_anal_stackop_tostring (op.stackop));
			}
			if (op.stackptr) {
				printline ("stackptr", "%"PFMT64u"\n", op.stackptr);
			}
		}
		//r_cons_printf ("false: 0x%08"PFMT64x"\n", core->offset+idx);
		//free (hint);
		free (mnem);
		r_anal_hint_free (hint);
		r_anal_op_fini (&op);
	}
	r_anal_op_fini (&op);
	if (fmt == 's') {
		r_cons_printf ("%d\n", totalsize);
	} else if (fmt == 'j') {
		pj_end (pj);
		r_cons_println (pj_string (pj));
		pj_free (pj);
	}
	r_anal_esil_free (esil);
}