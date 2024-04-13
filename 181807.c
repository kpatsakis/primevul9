R_API char *cmd_syscall_dostr(RCore *core, st64 n, ut64 addr) {
	int i;
	char str[64];
	st64 N = n;
	int defVector = r_syscall_get_swi (core->anal->syscall);
	if (defVector > 0) {
		n = -1;
	}
	if (n == -1 || defVector > 0) {
		n = (int)r_debug_reg_get (core->dbg, "oeax");
		if (!n || n == -1) {
			const char *a0 = r_reg_get_name (core->anal->reg, R_REG_NAME_SN);
			n = (a0 == NULL)? -1: (int)r_debug_reg_get (core->dbg, a0);
		}
	}
	RSyscallItem *item = r_syscall_get (core->anal->syscall, n, defVector);
	if (!item) {
		item =  r_syscall_get (core->anal->syscall, N, -1);
	}
	if (!item) {
		return r_str_newf ("%s = unknown ()", syscallNumber (n));
	}
	char *res = r_str_newf ("%s = %s (", syscallNumber (item->num), item->name);
	// TODO: move this to r_syscall
	//TODO replace the hardcoded CC with the sdb ones
	for (i = 0; i < item->args; i++) {
		// XXX this is a hack to make syscall args work on x86-32 and x86-64
		// we need to shift sn first.. which is bad, but needs to be redesigned
		int regidx = i;
		if (core->assembler->bits == 32) {
			regidx++;
		}
		ut64 arg = r_debug_arg_get (core->dbg, R_ANAL_CC_TYPE_FASTCALL, regidx);
		//r_cons_printf ("(%d:0x%"PFMT64x")\n", i, arg);
		if (item->sargs) {
			switch (item->sargs[i]) {
			case 'p': // pointer
				res = r_str_appendf (res, "0x%08" PFMT64x "", arg);
				break;
			case 'i':
				res = r_str_appendf (res, "%" PFMT64u "", arg);
				break;
			case 'z':
				memset (str, 0, sizeof (str));
				r_io_read_at (core->io, arg, (ut8 *)str, sizeof (str) - 1);
				r_str_filter (str, strlen (str));
				res = r_str_appendf (res, "\"%s\"", str);
				break;
			case 'Z': {
				//TODO replace the hardcoded CC with the sdb ones
				ut64 len = r_debug_arg_get (core->dbg, R_ANAL_CC_TYPE_FASTCALL, i + 2);
				len = R_MIN (len + 1, sizeof (str) - 1);
				if (len == 0) {
					len = 16; // override default
				}
				(void)r_io_read_at (core->io, arg, (ut8 *)str, len);
				str[len] = 0;
				r_str_filter (str, -1);
				res = r_str_appendf (res, "\"%s\"", str);
			} break;
			default:
				res = r_str_appendf (res, "0x%08" PFMT64x "", arg);
				break;
			}
		} else {
			res = r_str_appendf (res, "0x%08" PFMT64x "", arg);
		}
		if (i + 1 < item->args) {
			res = r_str_appendf (res, ", ");
		}
	}
	r_syscall_item_free (item);
	return r_str_appendf (res, ")");
}