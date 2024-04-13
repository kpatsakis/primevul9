static void initialize_stack (RCore *core, ut64 addr, ut64 size) {
	const char *mode = r_config_get (core->config, "esil.fillstack");
	if (mode && *mode && *mode != '0') {
		const ut64 bs = 4096 * 32;
		ut64 i;
		for (i = 0; i < size; i += bs) {
			ut64 left = R_MIN (bs, size - i);
		//	r_core_cmdf (core, "wx 10203040 @ 0x%llx", addr);
			switch (*mode) {
			case 'd': // "debrujn"
				r_core_cmdf (core, "wopD %"PFMT64u" @ 0x%"PFMT64x, left, addr + i);
				break;
			case 's': // "seq"
				r_core_cmdf (core, "woe 1 0xff 1 4 @ 0x%"PFMT64x"!0x%"PFMT64x, addr + i, left);
				break;
			case 'r': // "random"
				r_core_cmdf (core, "woR %"PFMT64u" @ 0x%"PFMT64x"!0x%"PFMT64x, left, addr + i, left);
				break;
			case 'z': // "zero"
			case '0':
				r_core_cmdf (core, "wow 00 @ 0x%"PFMT64x"!0x%"PFMT64x, addr + i, left);
				break;
			}
		}
		// eprintf ("[*] Initializing ESIL stack with pattern\n");
		// r_core_cmdf (core, "woe 0 10 4 @ 0x%"PFMT64x, size, addr);
	}
}