static void hint_node_print(HintNode *node, int mode, PJ *pj) {
	switch (mode) {
	case '*':
#define HINTCMD_ADDR(hint,fmt,x) r_cons_printf (fmt" @ 0x%"PFMT64x"\n", x, (hint)->addr)
		switch (node->type) {
		case HINT_NODE_ADDR: {
			const RAnalAddrHintRecord *record;
			r_vector_foreach (node->addr_hints, record) {
				switch (record->type) {
				case R_ANAL_ADDR_HINT_TYPE_IMMBASE:
					HINTCMD_ADDR (node, "ahi %d", record->immbase);
					break;
				case R_ANAL_ADDR_HINT_TYPE_JUMP:
					HINTCMD_ADDR (node, "ahc 0x%"PFMT64x, record->jump);
					break;
				case R_ANAL_ADDR_HINT_TYPE_FAIL:
					HINTCMD_ADDR (node, "ahf 0x%"PFMT64x, record->fail);
					break;
				case R_ANAL_ADDR_HINT_TYPE_STACKFRAME:
					HINTCMD_ADDR (node, "ahF 0x%"PFMT64x, record->stackframe);
					break;
				case R_ANAL_ADDR_HINT_TYPE_PTR:
					HINTCMD_ADDR (node, "ahp 0x%"PFMT64x, record->ptr);
					break;
				case R_ANAL_ADDR_HINT_TYPE_NWORD:
					// no command for this
					break;
				case R_ANAL_ADDR_HINT_TYPE_RET:
					HINTCMD_ADDR (node, "ahr 0x%"PFMT64x, record->retval);
					break;
				case R_ANAL_ADDR_HINT_TYPE_NEW_BITS:
					// no command for this
					break;
				case R_ANAL_ADDR_HINT_TYPE_SIZE:
					HINTCMD_ADDR (node, "ahs 0x%"PFMT64x, record->size);
					break;
				case R_ANAL_ADDR_HINT_TYPE_SYNTAX:
					HINTCMD_ADDR (node, "ahS %s", record->syntax); // TODO: escape for newcmd
					break;
				case R_ANAL_ADDR_HINT_TYPE_OPTYPE: {
					const char *type = r_anal_optype_to_string (record->optype);
					if (type) {
						HINTCMD_ADDR (node, "aho %s", type); // TODO: escape for newcmd
					}
					break;
				}
				case R_ANAL_ADDR_HINT_TYPE_OPCODE:
					HINTCMD_ADDR (node, "ahd %s", record->opcode);
					break;
				case R_ANAL_ADDR_HINT_TYPE_TYPE_OFFSET:
					HINTCMD_ADDR (node, "aht %s", record->type_offset); // TODO: escape for newcmd
					break;
				case R_ANAL_ADDR_HINT_TYPE_ESIL:
					HINTCMD_ADDR (node, "ahe %s", record->esil); // TODO: escape for newcmd
					break;
				case R_ANAL_ADDR_HINT_TYPE_HIGH:
					r_cons_printf ("ahh @ 0x%"PFMT64x"\n", node->addr);
					break;
				case R_ANAL_ADDR_HINT_TYPE_VAL:
					// no command for this
					break;
				}
			}
			break;
		}
		case HINT_NODE_ARCH:
			HINTCMD_ADDR (node, "aha %s", r_str_get_fail (node->arch, "0"));
			break;
		case HINT_NODE_BITS:
			HINTCMD_ADDR (node, "ahb %d", node->bits);
			break;
		}
#undef HINTCMD_ADDR
		break;
	case 'j':
		switch (node->type) {
		case HINT_NODE_ADDR: {
			const RAnalAddrHintRecord *record;
			r_vector_foreach (node->addr_hints, record) {
				switch (record->type) {
				case R_ANAL_ADDR_HINT_TYPE_IMMBASE:
					pj_ki (pj, "immbase", record->immbase);
					break;
				case R_ANAL_ADDR_HINT_TYPE_JUMP:
					pj_kn (pj, "jump", record->jump);
					break;
				case R_ANAL_ADDR_HINT_TYPE_FAIL:
					pj_kn (pj, "fail", record->fail);
					break;
				case R_ANAL_ADDR_HINT_TYPE_STACKFRAME:
					pj_kn (pj, "stackframe", record->stackframe);
					break;
				case R_ANAL_ADDR_HINT_TYPE_PTR:
					pj_kn (pj, "ptr", record->ptr);
					break;
				case R_ANAL_ADDR_HINT_TYPE_NWORD:
					pj_ki (pj, "nword", record->nword);
					break;
				case R_ANAL_ADDR_HINT_TYPE_RET:
					pj_kn (pj, "ret", record->retval);
					break;
				case R_ANAL_ADDR_HINT_TYPE_NEW_BITS:
					pj_ki (pj, "newbits", record->newbits);
					break;
				case R_ANAL_ADDR_HINT_TYPE_SIZE:
					pj_kn (pj, "size", record->size);
					break;
				case R_ANAL_ADDR_HINT_TYPE_SYNTAX:
					pj_ks (pj, "syntax", record->syntax);
					break;
				case R_ANAL_ADDR_HINT_TYPE_OPTYPE: {
					const char *type = r_anal_optype_to_string (record->optype);
					if (type) {
						pj_ks (pj, "type", type);
					}
					break;
				}
				case R_ANAL_ADDR_HINT_TYPE_OPCODE:
					pj_ks (pj, "opcode", record->opcode);
					break;
				case R_ANAL_ADDR_HINT_TYPE_TYPE_OFFSET:
					pj_ks (pj, "offset", record->type_offset);
					break;
				case R_ANAL_ADDR_HINT_TYPE_ESIL:
					pj_ks (pj, "esil", record->esil);
					break;
				case R_ANAL_ADDR_HINT_TYPE_HIGH:
					pj_kb (pj, "high", true);
					break;
				case R_ANAL_ADDR_HINT_TYPE_VAL:
					pj_kn (pj, "val", record->val);
					break;
				}
			}
			break;
		}
		case HINT_NODE_ARCH:
			if (node->arch) {
				pj_ks (pj, "arch", node->arch);
			} else {
				pj_knull (pj, "arch");
			}
			break;
		case HINT_NODE_BITS:
			pj_ki (pj, "bits", node->bits);
			break;
		}
		break;
	default:
		print_hint_h_format (node);
		break;
	}
}