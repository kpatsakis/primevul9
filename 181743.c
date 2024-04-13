static char *get_buf_asm(RCore *core, ut64 from, ut64 addr, RAnalFunction *fcn, bool color) {
	int has_color = core->print->flags & R_PRINT_FLAGS_COLOR;
	char str[512];
	const int size = 12;
	ut8 buf[12];
	RAsmOp asmop = {0};
	char *buf_asm = NULL;
	bool asm_varsub = r_config_get_i (core->config, "asm.var.sub");
	core->parser->pseudo = r_config_get_i (core->config, "asm.pseudo");
	core->parser->relsub = r_config_get_i (core->config, "asm.relsub");
	core->parser->localvar_only = r_config_get_i (core->config, "asm.var.subonly");

	if (core->parser->relsub) {
		core->parser->relsub_addr = from;
	}
	r_io_read_at (core->io, addr, buf, size);
	r_asm_set_pc (core->assembler, addr);
	r_asm_disassemble (core->assembler, &asmop, buf, size);
	int ba_len = r_strbuf_length (&asmop.buf_asm) + 128;
	char *ba = malloc (ba_len);
	strcpy (ba, r_strbuf_get (&asmop.buf_asm));
	if (asm_varsub) {
		r_parse_varsub (core->parser, fcn, addr, asmop.size,
				ba, ba, sizeof (asmop.buf_asm));
	}
	RAnalHint *hint = r_anal_hint_get (core->anal, addr);
	r_parse_filter (core->parser, addr, core->flags, hint,
			ba, str, sizeof (str), core->print->big_endian);
	r_anal_hint_free (hint);
	r_asm_op_set_asm (&asmop, ba);
	free (ba);
	if (color && has_color) {
		buf_asm = r_print_colorize_opcode (core->print, str,
				core->cons->context->pal.reg, core->cons->context->pal.num, false, fcn ? fcn->addr : 0);
	} else {
		buf_asm = r_str_new (str);
	}
	return buf_asm;
}