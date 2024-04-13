void die(const char *str, struct pt_regs *regs, long err)
{
	unsigned long flags = oops_begin();
	int sig = SIGSEGV;

	if (__die(str, regs, err))
		sig = 0;
	oops_end(flags, regs, sig);
}