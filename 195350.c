void show_ip(struct pt_regs *regs, const char *loglvl)
{
#ifdef CONFIG_X86_32
	printk("%sEIP: %pS\n", loglvl, (void *)regs->ip);
#else
	printk("%sRIP: %04x:%pS\n", loglvl, (int)regs->cs, (void *)regs->ip);
#endif
	show_opcodes(regs, loglvl);
}