static void cmd_anal_trampoline(RCore *core, const char *input) {
	int bits = r_config_get_i (core->config, "asm.bits");
	char *p, *inp = strdup (input);
	p = strchr (inp, ' ');
	if (p) {
		*p = 0;
	}
	ut64 a = r_num_math (core->num, inp);
	ut64 b = p? r_num_math (core->num, p + 1): 0;
	free (inp);

	switch (bits) {
	case 32:
		print_trampolines (core, a, b, 4);
		break;
	case 64:
		print_trampolines (core, a, b, 8);
		break;
	}
}