static void adaptive_pebs_save_regs(struct pt_regs *regs,
				    struct pebs_gprs *gprs)
{
	regs->ax = gprs->ax;
	regs->bx = gprs->bx;
	regs->cx = gprs->cx;
	regs->dx = gprs->dx;
	regs->si = gprs->si;
	regs->di = gprs->di;
	regs->bp = gprs->bp;
	regs->sp = gprs->sp;
#ifndef CONFIG_X86_32
	regs->r8 = gprs->r8;
	regs->r9 = gprs->r9;
	regs->r10 = gprs->r10;
	regs->r11 = gprs->r11;
	regs->r12 = gprs->r12;
	regs->r13 = gprs->r13;
	regs->r14 = gprs->r14;
	regs->r15 = gprs->r15;
#endif
}