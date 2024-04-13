static void kvm_machine_check(void)
{
#if defined(CONFIG_X86_MCE)
	struct pt_regs regs = {
		.cs = 3, /* Fake ring 3 no matter what the guest ran on */
		.flags = X86_EFLAGS_IF,
	};

	do_machine_check(&regs, 0);
#endif
}