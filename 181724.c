static void cmd_syscall_do(RCore *core, st64 n, ut64 addr) {
	char *msg = cmd_syscall_dostr (core, n, addr);
	if (msg) {
		r_cons_println (msg);
		free (msg);
	}
}