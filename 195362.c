void show_opcodes(struct pt_regs *regs, const char *loglvl)
{
#define PROLOGUE_SIZE 42
#define EPILOGUE_SIZE 21
#define OPCODE_BUFSIZE (PROLOGUE_SIZE + 1 + EPILOGUE_SIZE)
	u8 opcodes[OPCODE_BUFSIZE];
	unsigned long prologue = regs->ip - PROLOGUE_SIZE;
	bool bad_ip;

	/*
	 * Make sure userspace isn't trying to trick us into dumping kernel
	 * memory by pointing the userspace instruction pointer at it.
	 */
	bad_ip = user_mode(regs) &&
		__chk_range_not_ok(prologue, OPCODE_BUFSIZE, TASK_SIZE_MAX);

	if (bad_ip || probe_kernel_read(opcodes, (u8 *)prologue,
					OPCODE_BUFSIZE)) {
		printk("%sCode: Bad RIP value.\n", loglvl);
	} else {
		printk("%sCode: %" __stringify(PROLOGUE_SIZE) "ph <%02x> %"
		       __stringify(EPILOGUE_SIZE) "ph\n", loglvl, opcodes,
		       opcodes[PROLOGUE_SIZE], opcodes + PROLOGUE_SIZE + 1);
	}
}