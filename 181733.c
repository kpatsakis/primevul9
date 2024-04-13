static void __anal_reg_list(RCore *core, int type, int bits, char mode) {
	if (mode == 'i') {
		r_core_debug_ri (core, core->anal->reg, 0);
		return;
	}
	RReg *hack = core->dbg->reg;
	const char *use_color;
	int use_colors = r_config_get_i (core->config, "scr.color");
	if (use_colors) {
#undef ConsP
#define ConsP(x) (core->cons && core->cons->context->pal.x)? core->cons->context->pal.x
		use_color = ConsP (creg) : Color_BWHITE;
	} else {
		use_color = NULL;
	}
	if (bits < 0) {
		// TODO Change the `size` argument of r_debug_reg_list to use -1 for any and 0 for anal->bits
		bits = 0;
	} else if (!bits) {
		bits = core->anal->bits;
	}
	int mode2 = mode;
	if (core->anal) {
		core->dbg->reg = core->anal->reg;
		if (core->anal->cur && core->anal->cur->arch) {
			/* workaround for thumb */
			if (!strcmp (core->anal->cur->arch, "arm") && bits == 16) {
				bits = 32;
			}
			/* workaround for 6502 */
			if (!strcmp (core->anal->cur->arch, "6502") && bits == 8) {
				mode2 = mode == 'j' ? 'J' : mode;
				if (mode == 'j') {
					r_cons_printf ("{");
				}
				r_debug_reg_list (core->dbg, R_REG_TYPE_GPR, 16, mode2, use_color); // XXX detect which one is current usage
				if (mode == 'j') {
					r_cons_printf (",");
				}
			}
			if (!strcmp (core->anal->cur->arch, "avr") && bits == 8) {
				mode2 = mode == 'j' ? 'J' : mode;
				if (mode == 'j') {
					r_cons_printf ("{");
				}
				r_debug_reg_list (core->dbg, R_REG_TYPE_GPR, 16, mode2, use_color); // XXX detect which one is current usage
				if (mode == 'j') {
					r_cons_printf (",");
				}
			}
		}
	}

	if (mode == '=') {
		int pcbits = 0;
		const char *pcname = r_reg_get_name (core->anal->reg, R_REG_NAME_PC);
		RRegItem *reg = r_reg_get (core->anal->reg, pcname, 0);
		if (bits != reg->size) {
			pcbits = reg->size;
		}
		if (pcbits) {
			r_debug_reg_list (core->dbg, R_REG_TYPE_GPR, pcbits, mode, use_color); // XXX detect which one is current usage
		}
	}
	r_debug_reg_list (core->dbg, type, bits, mode2, use_color);
	if (mode2 == 'J') {
		r_cons_print ("}\n");
	}
	core->dbg->reg = hack;
}