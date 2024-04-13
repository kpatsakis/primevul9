void show_iret_regs(struct pt_regs *regs)
{
	show_ip(regs, KERN_DEFAULT);
	printk(KERN_DEFAULT "RSP: %04x:%016lx EFLAGS: %08lx", (int)regs->ss,
		regs->sp, regs->flags);
}