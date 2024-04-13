R_API RList* r_core_anal_cycles(RCore *core, int ccl) {
	ut64 addr = core->offset;
	int depth = 0;
	RAnalOp *op = NULL;
	RAnalCycleFrame *prev = NULL, *cf = NULL;
	RAnalCycleHook *ch;
	RList *hooks = r_list_new ();
	if (!hooks) {
		return NULL;
	}
	cf = r_anal_cycle_frame_new ();
	r_cons_break_push (NULL, NULL);
	while (cf && !r_cons_is_breaked ()) {
		if ((op = r_core_anal_op (core, addr, R_ANAL_OP_MASK_BASIC)) && (op->cycles) && (ccl > 0)) {
			r_cons_clear_line (1);
			eprintf ("%i -- ", ccl);
			addr += op->size;
			switch (op->type) {
			case R_ANAL_OP_TYPE_JMP:
				addr = op->jump;
				ccl -= op->cycles;
				loganal (op->addr, addr, depth);
				break;
			case R_ANAL_OP_TYPE_UJMP:
			case R_ANAL_OP_TYPE_MJMP:
			case R_ANAL_OP_TYPE_UCALL:
			case R_ANAL_OP_TYPE_ICALL:
			case R_ANAL_OP_TYPE_RCALL:
			case R_ANAL_OP_TYPE_IRCALL:
				ch = R_NEW0 (RAnalCycleHook);
				ch->addr = op->addr;
				eprintf ("0x%08"PFMT64x" > ?\r", op->addr);
				ch->cycles = ccl;
				r_list_append (hooks, ch);
				ch = NULL;
				while (!ch && cf) {
					ch = r_list_pop (cf->hooks);
					if (ch) {
						addr = ch->addr;
						ccl = ch->cycles;
						free (ch);
					} else {
						r_anal_cycle_frame_free (cf);
						cf = prev;
						if (cf) {
							prev = cf->prev;
						}
					}
				}
				break;
			case R_ANAL_OP_TYPE_CJMP:
				ch = R_NEW0 (RAnalCycleHook);
				ch->addr = addr;
				ch->cycles = ccl - op->failcycles;
				r_list_push (cf->hooks, ch);
				ch = NULL;
				addr = op->jump;
				loganal (op->addr, addr, depth);
				break;
			case R_ANAL_OP_TYPE_UCJMP:
			case R_ANAL_OP_TYPE_UCCALL:
				ch = R_NEW0 (RAnalCycleHook);
				ch->addr = op->addr;
				ch->cycles = ccl;
				r_list_append (hooks, ch);
				ch = NULL;
				ccl -= op->failcycles;
				eprintf ("0x%08"PFMT64x" > ?\r", op->addr);
				break;
			case R_ANAL_OP_TYPE_CCALL:
				ch = R_NEW0 (RAnalCycleHook);
				ch->addr = addr;
				ch->cycles = ccl - op->failcycles;
				r_list_push (cf->hooks, ch);
				ch = NULL;
			case R_ANAL_OP_TYPE_CALL:
				if (op->addr != op->jump) { //no selfies
					cf->naddr = addr;
					prev = cf;
					cf = r_anal_cycle_frame_new ();
					cf->prev = prev;
				}
				ccl -= op->cycles;
				addr = op->jump;
				loganal (op->addr, addr, depth - 1);
				break;
			case R_ANAL_OP_TYPE_RET:
				ch = R_NEW0 (RAnalCycleHook);
				if (prev) {
					ch->addr = prev->naddr;
					ccl -= op->cycles;
					ch->cycles = ccl;
					r_list_push (prev->hooks, ch);
					eprintf ("0x%08"PFMT64x" < 0x%08"PFMT64x"\r", prev->naddr, op->addr);
				} else {
					ch->addr = op->addr;
					ch->cycles = ccl;
					r_list_append (hooks, ch);
					eprintf ("? < 0x%08"PFMT64x"\r", op->addr);
				}
				ch = NULL;
				while (!ch && cf) {
					ch = r_list_pop (cf->hooks);
					if (ch) {
						addr = ch->addr;
						ccl = ch->cycles;
						free (ch);
					} else {
						r_anal_cycle_frame_free (cf);
						cf = prev;
						if (cf) {
							prev = cf->prev;
						}
					}
				}
				break;
			case R_ANAL_OP_TYPE_CRET:
				ch = R_NEW0 (RAnalCycleHook);
				if (prev) {
					ch->addr = prev->naddr;
					ch->cycles = ccl - op->cycles;
					r_list_push (prev->hooks, ch);
					eprintf ("0x%08"PFMT64x" < 0x%08"PFMT64x"\r", prev->naddr, op->addr);
				} else {
					ch->addr = op->addr;
					ch->cycles = ccl - op->cycles;
					r_list_append (hooks, ch);
					eprintf ("? < 0x%08"PFMT64x"\r", op->addr);
				}
				ccl -= op->failcycles;
				break;
			default:
				ccl -= op->cycles;
				eprintf ("0x%08"PFMT64x"\r", op->addr);
				break;
			}
		} else {
			ch = R_NEW0 (RAnalCycleHook);
			if (!ch) {
				r_anal_cycle_frame_free (cf);
				r_list_free (hooks);
				return NULL;
			}
			ch->addr = addr;
			ch->cycles = ccl;
			r_list_append (hooks, ch);
			ch = NULL;
			while (!ch && cf) {
				ch = r_list_pop (cf->hooks);
				if (ch) {
					addr = ch->addr;
					ccl = ch->cycles;
					free (ch);
				} else {
					r_anal_cycle_frame_free (cf);
					cf = prev;
					if (cf) {
						prev = cf->prev;
					}
				}
			}
		}
		r_anal_op_free (op);
	}
	if (r_cons_is_breaked ()) {
		while (cf) {
			ch = r_list_pop (cf->hooks);
			while (ch) {
				free (ch);
				ch = r_list_pop (cf->hooks);
			}
			prev = cf->prev;
			r_anal_cycle_frame_free (cf);
			cf = prev;
		}
	}
	r_cons_break_pop ();
	return hooks;
}