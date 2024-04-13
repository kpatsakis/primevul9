static void print_hint_h_format(HintNode *node) {
	switch (node->type) {
	case HINT_NODE_ADDR: {
		const RAnalAddrHintRecord *record;
		r_vector_foreach (node->addr_hints, record) {
			switch (record->type) {
			case R_ANAL_ADDR_HINT_TYPE_IMMBASE:
				r_cons_printf (" immbase=%d", record->immbase);
				break;
			case R_ANAL_ADDR_HINT_TYPE_JUMP:
				r_cons_printf (" jump=0x%08"PFMT64x, record->jump);
				break;
			case R_ANAL_ADDR_HINT_TYPE_FAIL:
				r_cons_printf (" fail=0x%08"PFMT64x, record->fail);
				break;
			case R_ANAL_ADDR_HINT_TYPE_STACKFRAME:
				r_cons_printf (" stackframe=0x%"PFMT64x, record->stackframe);
				break;
			case R_ANAL_ADDR_HINT_TYPE_PTR:
				r_cons_printf (" ptr=0x%"PFMT64x, record->ptr);
				break;
			case R_ANAL_ADDR_HINT_TYPE_NWORD:
				r_cons_printf (" nword=%d", record->nword);
				break;
			case R_ANAL_ADDR_HINT_TYPE_RET:
				r_cons_printf (" ret=0x%08"PFMT64x, record->retval);
				break;
			case R_ANAL_ADDR_HINT_TYPE_NEW_BITS:
				r_cons_printf (" newbits=%d", record->newbits);
				break;
			case R_ANAL_ADDR_HINT_TYPE_SIZE:
				r_cons_printf (" size=%"PFMT64u, record->size);
				break;
			case R_ANAL_ADDR_HINT_TYPE_SYNTAX:
				r_cons_printf (" syntax='%s'", record->syntax);
				break;
			case R_ANAL_ADDR_HINT_TYPE_OPTYPE: {
				const char *type = r_anal_optype_to_string (record->optype);
				if (type) {
					r_cons_printf (" type='%s'", type);
				}
				break;
			}
			case R_ANAL_ADDR_HINT_TYPE_OPCODE:
				r_cons_printf (" opcode='%s'", record->opcode);
				break;
			case R_ANAL_ADDR_HINT_TYPE_TYPE_OFFSET:
				r_cons_printf (" offset='%s'", record->type_offset);
				break;
			case R_ANAL_ADDR_HINT_TYPE_ESIL:
				r_cons_printf (" esil='%s'", record->esil);
				break;
			case R_ANAL_ADDR_HINT_TYPE_HIGH:
				r_cons_printf (" high=true");
				break;
			case R_ANAL_ADDR_HINT_TYPE_VAL:
				r_cons_printf (" val=0x%08"PFMT64x, record->val);
				break;
			}
		}
		break;
	}
	case HINT_NODE_ARCH:
		if (node->arch) {
			r_cons_printf (" arch='%s'", node->arch);
		} else {
			r_cons_print (" arch=RESET");
		}
		break;
	case HINT_NODE_BITS:
		if (node->bits) {
			r_cons_printf (" bits=%d", node->bits);
		} else {
			r_cons_print (" bits=RESET");
		}
		break;
	}
}